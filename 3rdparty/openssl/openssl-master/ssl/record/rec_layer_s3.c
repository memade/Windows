/*
 * Copyright 1995-2022 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include "../ssl_local.h"
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>
#include <openssl/core_names.h>
#include "record_local.h"
#include "internal/packet.h"

#if     defined(OPENSSL_SMALL_FOOTPRINT) || \
        !(      defined(AES_ASM) &&     ( \
                defined(__x86_64)       || defined(__x86_64__)  || \
                defined(_M_AMD64)       || defined(_M_X64)      ) \
        )
# undef EVP_CIPH_FLAG_TLS1_1_MULTIBLOCK
# define EVP_CIPH_FLAG_TLS1_1_MULTIBLOCK 0
#endif

void RECORD_LAYER_init(RECORD_LAYER *rl, SSL_CONNECTION *s)
{
    rl->s = s;
}

void RECORD_LAYER_clear(RECORD_LAYER *rl)
{
    rl->wnum = 0;
    memset(rl->handshake_fragment, 0, sizeof(rl->handshake_fragment));
    rl->handshake_fragment_len = 0;
    rl->wpend_tot = 0;
    rl->wpend_type = 0;
    rl->wpend_ret = 0;
    rl->wpend_buf = NULL;

    ssl3_release_write_buffer(rl->s);

    RECORD_LAYER_reset_write_sequence(rl);

    if (rl->rrlmethod != NULL)
        rl->rrlmethod->free(rl->rrl); /* Ignore return value */
    BIO_free(rl->rrlnext);
    rl->rrlmethod = NULL;
    rl->rrlnext = NULL;

    if (rl->d)
        DTLS_RECORD_LAYER_clear(rl);
}

void RECORD_LAYER_release(RECORD_LAYER *rl)
{
    if (rl->numwpipes > 0)
        ssl3_release_write_buffer(rl->s);
}

/* Checks if we have unprocessed read ahead data pending */
int RECORD_LAYER_read_pending(const RECORD_LAYER *rl)
{
    return rl->rrlmethod->unprocessed_read_pending(rl->rrl);
}

/* Checks if we have decrypted unread record data pending */
int RECORD_LAYER_processed_read_pending(const RECORD_LAYER *rl)
{
    return (rl->curr_rec < rl->num_recs)
           || rl->rrlmethod->processed_read_pending(rl->rrl);
}

int RECORD_LAYER_write_pending(const RECORD_LAYER *rl)
{
    return (rl->numwpipes > 0)
        && SSL3_BUFFER_get_left(&rl->wbuf[rl->numwpipes - 1]) != 0;
}

void RECORD_LAYER_reset_write_sequence(RECORD_LAYER *rl)
{
    memset(rl->write_sequence, 0, sizeof(rl->write_sequence));
}

size_t ssl3_pending(const SSL *s)
{
    size_t i, num = 0;
    const SSL_CONNECTION *sc = SSL_CONNECTION_FROM_CONST_SSL(s);

    if (sc == NULL)
        return 0;

    if (SSL_CONNECTION_IS_DTLS(sc)) {
        TLS_RECORD *rdata;
        pitem *item, *iter;

        iter = pqueue_iterator(sc->rlayer.d->buffered_app_data.q);
        while ((item = pqueue_next(&iter)) != NULL) {
            rdata = item->data;
            num += rdata->length;
        }
    }

    for (i = 0; i < sc->rlayer.num_recs; i++) {
        if (sc->rlayer.tlsrecs[i].type != SSL3_RT_APPLICATION_DATA)
            return num;
        num += sc->rlayer.tlsrecs[i].length;
    }

    num += sc->rlayer.rrlmethod->app_data_pending(sc->rlayer.rrl);

    return num;
}

void SSL_CTX_set_default_read_buffer_len(SSL_CTX *ctx, size_t len)
{
    ctx->default_read_buf_len = len;
}

void SSL_set_default_read_buffer_len(SSL *s, size_t len)
{
    SSL_CONNECTION *sc = SSL_CONNECTION_FROM_SSL(s);

    if (sc == NULL)
        return;
    sc->rlayer.default_read_buf_len = len;
}

const char *SSL_rstate_string_long(const SSL *s)
{
    const SSL_CONNECTION *sc = SSL_CONNECTION_FROM_CONST_SSL(s);
    const char *lng;

    if (sc == NULL)
        return NULL;

    if (sc->rlayer.rrlmethod == NULL || sc->rlayer.rrl == NULL)
        return "unknown";

    sc->rlayer.rrlmethod->get_state(sc->rlayer.rrl, NULL, &lng);

    return lng;
}

const char *SSL_rstate_string(const SSL *s)
{
    const SSL_CONNECTION *sc = SSL_CONNECTION_FROM_CONST_SSL(s);
    const char *shrt;

    if (sc == NULL)
        return NULL;

    if (sc->rlayer.rrlmethod == NULL || sc->rlayer.rrl == NULL)
        return "unknown";

    sc->rlayer.rrlmethod->get_state(sc->rlayer.rrl, &shrt, NULL);

    return shrt;
}

/*
 * Call this to write data in records of type 'type' It will return <= 0 if
 * not all data has been sent or non-blocking IO.
 */
int ssl3_write_bytes(SSL *ssl, int type, const void *buf_, size_t len,
                     size_t *written)
{
    const unsigned char *buf = buf_;
    size_t tot;
    size_t n, max_send_fragment, split_send_fragment, maxpipes;
#if !defined(OPENSSL_NO_MULTIBLOCK) && EVP_CIPH_FLAG_TLS1_1_MULTIBLOCK
    size_t nw;
#endif
    SSL3_BUFFER *wb;
    int i;
    size_t tmpwrit;
    SSL_CONNECTION *s = SSL_CONNECTION_FROM_SSL_ONLY(ssl);

    if (s == NULL)
        return -1;

    wb = &s->rlayer.wbuf[0];
    s->rwstate = SSL_NOTHING;
    tot = s->rlayer.wnum;
    /*
     * ensure that if we end up with a smaller value of data to write out
     * than the original len from a write which didn't complete for
     * non-blocking I/O and also somehow ended up avoiding the check for
     * this in ssl3_write_pending/SSL_R_BAD_WRITE_RETRY as it must never be
     * possible to end up with (len-tot) as a large number that will then
     * promptly send beyond the end of the users buffer ... so we trap and
     * report the error in a way the user will notice
     */
    if ((len < s->rlayer.wnum)
        || ((wb->left != 0) && (len < (s->rlayer.wnum + s->rlayer.wpend_tot)))) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_R_BAD_LENGTH);
        return -1;
    }

    if (s->early_data_state == SSL_EARLY_DATA_WRITING
            && !ossl_early_data_count_ok(s, len, 0, 1)) {
        /* SSLfatal() already called */
        return -1;
    }

    s->rlayer.wnum = 0;

    /*
     * If we are supposed to be sending a KeyUpdate or NewSessionTicket then go
     * into init unless we have writes pending - in which case we should finish
     * doing that first.
     */
    if (wb->left == 0 && (s->key_update != SSL_KEY_UPDATE_NONE
                          || s->ext.extra_tickets_expected > 0))
        ossl_statem_set_in_init(s, 1);

    /*
     * When writing early data on the server side we could be "in_init" in
     * between receiving the EoED and the CF - but we don't want to handle those
     * messages yet.
     */
    if (SSL_in_init(ssl) && !ossl_statem_get_in_handshake(s)
            && s->early_data_state != SSL_EARLY_DATA_UNAUTH_WRITING) {
        i = s->handshake_func(ssl);
        /* SSLfatal() already called */
        if (i < 0)
            return i;
        if (i == 0) {
            return -1;
        }
    }

    /*
     * first check if there is a SSL3_BUFFER still being written out.  This
     * will happen with non blocking IO
     */
    if (wb->left != 0) {
        /* SSLfatal() already called if appropriate */
        i = ssl3_write_pending(s, type, &buf[tot], s->rlayer.wpend_tot,
                               &tmpwrit);
        if (i <= 0) {
            /* XXX should we ssl3_release_write_buffer if i<0? */
            s->rlayer.wnum = tot;
            return i;
        }
        tot += tmpwrit;               /* this might be last fragment */
    }
#if !defined(OPENSSL_NO_MULTIBLOCK) && EVP_CIPH_FLAG_TLS1_1_MULTIBLOCK
    /*
     * Depending on platform multi-block can deliver several *times*
     * better performance. Downside is that it has to allocate
     * jumbo buffer to accommodate up to 8 records, but the
     * compromise is considered worthy.
     */
    if (type == SSL3_RT_APPLICATION_DATA
            && len >= 4 * (max_send_fragment = ssl_get_max_send_fragment(s))
            && s->compress == NULL
            && s->msg_callback == NULL
            && !SSL_WRITE_ETM(s)
            && SSL_USE_EXPLICIT_IV(s)
            && !BIO_get_ktls_send(s->wbio)
            && (EVP_CIPHER_get_flags(EVP_CIPHER_CTX_get0_cipher(s->enc_write_ctx))
                & EVP_CIPH_FLAG_TLS1_1_MULTIBLOCK) != 0) {
        unsigned char aad[13];
        EVP_CTRL_TLS1_1_MULTIBLOCK_PARAM mb_param;
        size_t packlen;
        int packleni;

        /* minimize address aliasing conflicts */
        if ((max_send_fragment & 0xfff) == 0)
            max_send_fragment -= 512;

        if (tot == 0 || wb->buf == NULL) { /* allocate jumbo buffer */
            ssl3_release_write_buffer(s);

            packlen = EVP_CIPHER_CTX_ctrl(s->enc_write_ctx,
                                          EVP_CTRL_TLS1_1_MULTIBLOCK_MAX_BUFSIZE,
                                          (int)max_send_fragment, NULL);

            if (len >= 8 * max_send_fragment)
                packlen *= 8;
            else
                packlen *= 4;

            if (!ssl3_setup_write_buffer(s, 1, packlen)) {
                /* SSLfatal() already called */
                return -1;
            }
        } else if (tot == len) { /* done? */
            /* free jumbo buffer */
            ssl3_release_write_buffer(s);
            *written = tot;
            return 1;
        }

        n = (len - tot);
        for (;;) {
            if (n < 4 * max_send_fragment) {
                /* free jumbo buffer */
                ssl3_release_write_buffer(s);
                break;
            }

            if (s->s3.alert_dispatch) {
                i = ssl->method->ssl_dispatch_alert(ssl);
                if (i <= 0) {
                    /* SSLfatal() already called if appropriate */
                    s->rlayer.wnum = tot;
                    return i;
                }
            }

            if (n >= 8 * max_send_fragment)
                nw = max_send_fragment * (mb_param.interleave = 8);
            else
                nw = max_send_fragment * (mb_param.interleave = 4);

            memcpy(aad, s->rlayer.write_sequence, 8);
            aad[8] = type;
            aad[9] = (unsigned char)(s->version >> 8);
            aad[10] = (unsigned char)(s->version);
            aad[11] = 0;
            aad[12] = 0;
            mb_param.out = NULL;
            mb_param.inp = aad;
            mb_param.len = nw;

            packleni = EVP_CIPHER_CTX_ctrl(s->enc_write_ctx,
                                          EVP_CTRL_TLS1_1_MULTIBLOCK_AAD,
                                          sizeof(mb_param), &mb_param);
            packlen = (size_t)packleni;
            if (packleni <= 0 || packlen > wb->len) { /* never happens */
                /* free jumbo buffer */
                ssl3_release_write_buffer(s);
                break;
            }

            mb_param.out = wb->buf;
            mb_param.inp = &buf[tot];
            mb_param.len = nw;

            if (EVP_CIPHER_CTX_ctrl(s->enc_write_ctx,
                                    EVP_CTRL_TLS1_1_MULTIBLOCK_ENCRYPT,
                                    sizeof(mb_param), &mb_param) <= 0)
                return -1;

            s->rlayer.write_sequence[7] += mb_param.interleave;
            if (s->rlayer.write_sequence[7] < mb_param.interleave) {
                int j = 6;
                while (j >= 0 && (++s->rlayer.write_sequence[j--]) == 0) ;
            }

            wb->offset = 0;
            wb->left = packlen;

            s->rlayer.wpend_tot = nw;
            s->rlayer.wpend_buf = &buf[tot];
            s->rlayer.wpend_type = type;
            s->rlayer.wpend_ret = nw;

            i = ssl3_write_pending(s, type, &buf[tot], nw, &tmpwrit);
            if (i <= 0) {
                /* SSLfatal() already called if appropriate */
                if (i < 0 && (!s->wbio || !BIO_should_retry(s->wbio))) {
                    /* free jumbo buffer */
                    ssl3_release_write_buffer(s);
                }
                s->rlayer.wnum = tot;
                return i;
            }
            if (tmpwrit == n) {
                /* free jumbo buffer */
                ssl3_release_write_buffer(s);
                *written = tot + tmpwrit;
                return 1;
            }
            n -= tmpwrit;
            tot += tmpwrit;
        }
    } else
#endif  /* !defined(OPENSSL_NO_MULTIBLOCK) && EVP_CIPH_FLAG_TLS1_1_MULTIBLOCK */
    if (tot == len) {           /* done? */
        if (s->mode & SSL_MODE_RELEASE_BUFFERS && !SSL_CONNECTION_IS_DTLS(s))
            ssl3_release_write_buffer(s);

        *written = tot;
        return 1;
    }

    n = (len - tot);

    max_send_fragment = ssl_get_max_send_fragment(s);
    split_send_fragment = ssl_get_split_send_fragment(s);
    /*
     * If max_pipelines is 0 then this means "undefined" and we default to
     * 1 pipeline. Similarly if the cipher does not support pipelined
     * processing then we also only use 1 pipeline, or if we're not using
     * explicit IVs
     */
    maxpipes = s->max_pipelines;
    if (maxpipes > SSL_MAX_PIPELINES) {
        /*
         * We should have prevented this when we set max_pipelines so we
         * shouldn't get here
         */
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
        return -1;
    }
    if (maxpipes == 0
        || s->enc_write_ctx == NULL
        || (EVP_CIPHER_get_flags(EVP_CIPHER_CTX_get0_cipher(s->enc_write_ctx))
            & EVP_CIPH_FLAG_PIPELINE) == 0
        || !SSL_USE_EXPLICIT_IV(s))
        maxpipes = 1;
    if (max_send_fragment == 0
            || split_send_fragment == 0
            || split_send_fragment > max_send_fragment) {
        /*
         * We should have prevented this when we set/get the split and max send
         * fragments so we shouldn't get here
         */
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
        return -1;
    }

    for (;;) {
        size_t pipelens[SSL_MAX_PIPELINES], tmppipelen, remain;
        size_t numpipes, j;

        if (n == 0)
            numpipes = 1;
        else
            numpipes = ((n - 1) / split_send_fragment) + 1;
        if (numpipes > maxpipes)
            numpipes = maxpipes;

        if (n / numpipes >= max_send_fragment) {
            /*
             * We have enough data to completely fill all available
             * pipelines
             */
            for (j = 0; j < numpipes; j++) {
                pipelens[j] = max_send_fragment;
            }
        } else {
            /* We can partially fill all available pipelines */
            tmppipelen = n / numpipes;
            remain = n % numpipes;
            for (j = 0; j < numpipes; j++) {
                pipelens[j] = tmppipelen;
                if (j < remain)
                    pipelens[j]++;
            }
        }

        i = do_ssl3_write(s, type, &(buf[tot]), pipelens, numpipes, 0,
                          &tmpwrit);
        if (i <= 0) {
            /* SSLfatal() already called if appropriate */
            /* XXX should we ssl3_release_write_buffer if i<0? */
            s->rlayer.wnum = tot;
            return i;
        }

        if (tmpwrit == n ||
            (type == SSL3_RT_APPLICATION_DATA &&
             (s->mode & SSL_MODE_ENABLE_PARTIAL_WRITE))) {
            /*
             * next chunk of data should get another prepended empty fragment
             * in ciphersuites with known-IV weakness:
             */
            s->s3.empty_fragment_done = 0;

            if (tmpwrit == n
                    && (s->mode & SSL_MODE_RELEASE_BUFFERS) != 0
                    && !SSL_CONNECTION_IS_DTLS(s))
                ssl3_release_write_buffer(s);

            *written = tot + tmpwrit;
            return 1;
        }

        n -= tmpwrit;
        tot += tmpwrit;
    }
}

int do_ssl3_write(SSL_CONNECTION *s, int type, const unsigned char *buf,
                  size_t *pipelens, size_t numpipes,
                  int create_empty_fragment, size_t *written)
{
    WPACKET pkt[SSL_MAX_PIPELINES];
    SSL3_RECORD wr[SSL_MAX_PIPELINES];
    WPACKET *thispkt;
    SSL3_RECORD *thiswr;
    unsigned char *recordstart;
    int i, mac_size, clear = 0;
    size_t prefix_len = 0;
    int eivlen = 0;
    size_t align = 0;
    SSL3_BUFFER *wb;
    SSL_SESSION *sess;
    size_t totlen = 0, len, wpinited = 0;
    size_t j;
    int using_ktls;
    SSL *ssl = SSL_CONNECTION_GET_SSL(s);

    for (j = 0; j < numpipes; j++)
        totlen += pipelens[j];
    /*
     * first check if there is a SSL3_BUFFER still being written out.  This
     * will happen with non blocking IO
     */
    if (RECORD_LAYER_write_pending(&s->rlayer)) {
        /* Calls SSLfatal() as required */
        return ssl3_write_pending(s, type, buf, totlen, written);
    }

    /* If we have an alert to send, lets send it */
    if (s->s3.alert_dispatch) {
        i = ssl->method->ssl_dispatch_alert(ssl);
        if (i <= 0) {
            /* SSLfatal() already called if appropriate */
            return i;
        }
        /* if it went, fall through and send more stuff */
    }

    if (s->rlayer.numwpipes < numpipes) {
        if (!ssl3_setup_write_buffer(s, numpipes, 0)) {
            /* SSLfatal() already called */
            return -1;
        }
    }

    if (totlen == 0 && !create_empty_fragment)
        return 0;

    sess = s->session;

    if ((sess == NULL)
            || (s->enc_write_ctx == NULL)
            || (EVP_MD_CTX_get0_md(s->write_hash) == NULL)) {
        clear = s->enc_write_ctx ? 0 : 1; /* must be AEAD cipher */
        mac_size = 0;
    } else {
        mac_size = EVP_MD_CTX_get_size(s->write_hash);
        if (mac_size < 0) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            goto err;
        }
    }

    /*
     * 'create_empty_fragment' is true only when this function calls itself
     */
    if (!clear && !create_empty_fragment && !s->s3.empty_fragment_done) {
        /*
         * countermeasure against known-IV weakness in CBC ciphersuites (see
         * http://www.openssl.org/~bodo/tls-cbc.txt)
         */

        if (s->s3.need_empty_fragments && type == SSL3_RT_APPLICATION_DATA) {
            /*
             * recursive function call with 'create_empty_fragment' set; this
             * prepares and buffers the data for an empty fragment (these
             * 'prefix_len' bytes are sent out later together with the actual
             * payload)
             */
            size_t tmppipelen = 0;
            int ret;

            ret = do_ssl3_write(s, type, buf, &tmppipelen, 1, 1, &prefix_len);
            if (ret <= 0) {
                /* SSLfatal() already called if appropriate */
                goto err;
            }

            if (prefix_len >
                (SSL3_RT_HEADER_LENGTH + SSL3_RT_SEND_MAX_ENCRYPTED_OVERHEAD)) {
                /* insufficient space */
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                goto err;
            }
        }

        s->s3.empty_fragment_done = 1;
    }

    using_ktls = BIO_get_ktls_send(s->wbio);
    if (using_ktls) {
        /*
         * ktls doesn't modify the buffer, but to avoid a warning we need to
         * discard the const qualifier.
         * This doesn't leak memory because the buffers have been released when
         * switching to ktls.
         */
        SSL3_BUFFER_set_buf(&s->rlayer.wbuf[0], (unsigned char *)buf);
        SSL3_BUFFER_set_offset(&s->rlayer.wbuf[0], 0);
        SSL3_BUFFER_set_app_buffer(&s->rlayer.wbuf[0], 1);
        goto wpacket_init_complete;
    }

    if (create_empty_fragment) {
        wb = &s->rlayer.wbuf[0];
#if defined(SSL3_ALIGN_PAYLOAD) && SSL3_ALIGN_PAYLOAD!=0
        /*
         * extra fragment would be couple of cipher blocks, which would be
         * multiple of SSL3_ALIGN_PAYLOAD, so if we want to align the real
         * payload, then we can just pretend we simply have two headers.
         */
        align = (size_t)SSL3_BUFFER_get_buf(wb) + 2 * SSL3_RT_HEADER_LENGTH;
        align = SSL3_ALIGN_PAYLOAD - 1 - ((align - 1) % SSL3_ALIGN_PAYLOAD);
#endif
        SSL3_BUFFER_set_offset(wb, align);
        if (!WPACKET_init_static_len(&pkt[0], SSL3_BUFFER_get_buf(wb),
                                     SSL3_BUFFER_get_len(wb), 0)
                || !WPACKET_allocate_bytes(&pkt[0], align, NULL)) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            goto err;
        }
        wpinited = 1;
    } else if (prefix_len) {
        wb = &s->rlayer.wbuf[0];
        if (!WPACKET_init_static_len(&pkt[0],
                                     SSL3_BUFFER_get_buf(wb),
                                     SSL3_BUFFER_get_len(wb), 0)
                || !WPACKET_allocate_bytes(&pkt[0], SSL3_BUFFER_get_offset(wb)
                                                    + prefix_len, NULL)) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            goto err;
        }
        wpinited = 1;
    } else {
        for (j = 0; j < numpipes; j++) {
            thispkt = &pkt[j];

            wb = &s->rlayer.wbuf[j];
#if defined(SSL3_ALIGN_PAYLOAD) && SSL3_ALIGN_PAYLOAD != 0
            align = (size_t)SSL3_BUFFER_get_buf(wb) + SSL3_RT_HEADER_LENGTH;
            align = SSL3_ALIGN_PAYLOAD - 1 - ((align - 1) % SSL3_ALIGN_PAYLOAD);
#endif
            SSL3_BUFFER_set_offset(wb, align);
            if (!WPACKET_init_static_len(thispkt, SSL3_BUFFER_get_buf(wb),
                                         SSL3_BUFFER_get_len(wb), 0)
                    || !WPACKET_allocate_bytes(thispkt, align, NULL)) {
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                goto err;
            }
            wpinited++;
        }
    }

    /* Explicit IV length, block ciphers appropriate version flag */
    if (s->enc_write_ctx && SSL_USE_EXPLICIT_IV(s)
        && !SSL_CONNECTION_TREAT_AS_TLS13(s)) {
        int mode = EVP_CIPHER_CTX_get_mode(s->enc_write_ctx);
        if (mode == EVP_CIPH_CBC_MODE) {
            eivlen = EVP_CIPHER_CTX_get_iv_length(s->enc_write_ctx);
            if (eivlen < 0) {
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_R_LIBRARY_BUG);
                goto err;
	    }
            if (eivlen <= 1)
                eivlen = 0;
        } else if (mode == EVP_CIPH_GCM_MODE) {
            /* Need explicit part of IV for GCM mode */
            eivlen = EVP_GCM_TLS_EXPLICIT_IV_LEN;
        } else if (mode == EVP_CIPH_CCM_MODE) {
            eivlen = EVP_CCM_TLS_EXPLICIT_IV_LEN;
        }
    }

 wpacket_init_complete:

    totlen = 0;
    /* Clear our SSL3_RECORD structures */
    memset(wr, 0, sizeof(wr));
    for (j = 0; j < numpipes; j++) {
        unsigned int version = (s->version == TLS1_3_VERSION) ? TLS1_2_VERSION
                                                              : s->version;
        unsigned char *compressdata = NULL;
        size_t maxcomplen;
        unsigned int rectype;

        thispkt = &pkt[j];
        thiswr = &wr[j];

        /*
         * In TLSv1.3, once encrypting, we always use application data for the
         * record type
         */
        if (SSL_CONNECTION_TREAT_AS_TLS13(s)
                && s->enc_write_ctx != NULL
                && (s->statem.enc_write_state != ENC_WRITE_STATE_WRITE_PLAIN_ALERTS
                    || type != SSL3_RT_ALERT))
            rectype = SSL3_RT_APPLICATION_DATA;
        else
            rectype = type;
        SSL3_RECORD_set_type(thiswr, rectype);

        /*
         * Some servers hang if initial client hello is larger than 256 bytes
         * and record version number > TLS 1.0
         */
        if (SSL_get_state(ssl) == TLS_ST_CW_CLNT_HELLO
                && !s->renegotiate
                && TLS1_get_version(ssl) > TLS1_VERSION
                && s->hello_retry_request == SSL_HRR_NONE)
            version = TLS1_VERSION;
        SSL3_RECORD_set_rec_version(thiswr, version);

        maxcomplen = pipelens[j];
        if (s->compress != NULL)
            maxcomplen += SSL3_RT_MAX_COMPRESSED_OVERHEAD;

        /*
         * When using offload kernel will write the header.
         * Otherwise write the header now
         */
        if (!using_ktls
                && (!WPACKET_put_bytes_u8(thispkt, rectype)
                || !WPACKET_put_bytes_u16(thispkt, version)
                || !WPACKET_start_sub_packet_u16(thispkt)
                || (eivlen > 0
                    && !WPACKET_allocate_bytes(thispkt, eivlen, NULL))
                || (maxcomplen > 0
                    && !WPACKET_reserve_bytes(thispkt, maxcomplen,
                                              &compressdata)))) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            goto err;
        }

        /* lets setup the record stuff. */
        SSL3_RECORD_set_data(thiswr, compressdata);
        SSL3_RECORD_set_length(thiswr, pipelens[j]);
        SSL3_RECORD_set_input(thiswr, (unsigned char *)&buf[totlen]);
        totlen += pipelens[j];

        /*
         * we now 'read' from thiswr->input, thiswr->length bytes into
         * thiswr->data
         */

        /* first we compress */
        if (s->compress != NULL) {
            if (!ssl3_do_compress(s, thiswr)
                    || !WPACKET_allocate_bytes(thispkt, thiswr->length, NULL)) {
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_R_COMPRESSION_FAILURE);
                goto err;
            }
        } else {
            if (using_ktls) {
                SSL3_RECORD_reset_data(&wr[j]);
            } else {
                if (!WPACKET_memcpy(thispkt, thiswr->input, thiswr->length)) {
                    SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                    goto err;
                }
                SSL3_RECORD_reset_input(&wr[j]);
            }
        }

        if (SSL_CONNECTION_TREAT_AS_TLS13(s)
                && !using_ktls
                && s->enc_write_ctx != NULL
                && (s->statem.enc_write_state != ENC_WRITE_STATE_WRITE_PLAIN_ALERTS
                    || type != SSL3_RT_ALERT)) {
            size_t rlen, max_send_fragment;

            if (!WPACKET_put_bytes_u8(thispkt, type)) {
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                goto err;
            }
            SSL3_RECORD_add_length(thiswr, 1);

            /* Add TLS1.3 padding */
            max_send_fragment = ssl_get_max_send_fragment(s);
            rlen = SSL3_RECORD_get_length(thiswr);
            if (rlen < max_send_fragment) {
                size_t padding = 0;
                size_t max_padding = max_send_fragment - rlen;
                if (s->record_padding_cb != NULL) {
                    padding = s->record_padding_cb(ssl, type, rlen, s->record_padding_arg);
                } else if (s->block_padding > 0) {
                    size_t mask = s->block_padding - 1;
                    size_t remainder;

                    /* optimize for power of 2 */
                    if ((s->block_padding & mask) == 0)
                        remainder = rlen & mask;
                    else
                        remainder = rlen % s->block_padding;
                    /* don't want to add a block of padding if we don't have to */
                    if (remainder == 0)
                        padding = 0;
                    else
                        padding = s->block_padding - remainder;
                }
                if (padding > 0) {
                    /* do not allow the record to exceed max plaintext length */
                    if (padding > max_padding)
                        padding = max_padding;
                    if (!WPACKET_memset(thispkt, 0, padding)) {
                        SSLfatal(s, SSL_AD_INTERNAL_ERROR,
                                 ERR_R_INTERNAL_ERROR);
                        goto err;
                    }
                    SSL3_RECORD_add_length(thiswr, padding);
                }
            }
        }

        /*
         * we should still have the output to thiswr->data and the input from
         * wr->input. Length should be thiswr->length. thiswr->data still points
         * in the wb->buf
         */

        if (!using_ktls && !SSL_WRITE_ETM(s) && mac_size != 0) {
            unsigned char *mac;

            if (!WPACKET_allocate_bytes(thispkt, mac_size, &mac)
                    || !ssl->method->ssl3_enc->mac(s, thiswr, mac, 1)) {
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                goto err;
            }
        }

        /*
         * Reserve some bytes for any growth that may occur during encryption.
         * This will be at most one cipher block or the tag length if using
         * AEAD. SSL_RT_MAX_CIPHER_BLOCK_SIZE covers either case.
         */
        if (!using_ktls) {
            if (!WPACKET_reserve_bytes(thispkt,
                                        SSL_RT_MAX_CIPHER_BLOCK_SIZE,
                                        NULL)
                /*
                 * We also need next the amount of bytes written to this
                 * sub-packet
                 */
                || !WPACKET_get_length(thispkt, &len)) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            goto err;
            }

            /* Get a pointer to the start of this record excluding header */
            recordstart = WPACKET_get_curr(thispkt) - len;
            SSL3_RECORD_set_data(thiswr, recordstart);
            SSL3_RECORD_reset_input(thiswr);
            SSL3_RECORD_set_length(thiswr, len);
        }
    }

    if (s->statem.enc_write_state == ENC_WRITE_STATE_WRITE_PLAIN_ALERTS) {
        /*
         * We haven't actually negotiated the version yet, but we're trying to
         * send early data - so we need to use the tls13enc function.
         */
        if (tls13_enc(s, wr, numpipes, 1, NULL, mac_size) < 1) {
            if (!ossl_statem_in_error(s)) {
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            }
            goto err;
        }
    } else {
        if (!using_ktls) {
            if (ssl->method->ssl3_enc->enc(s, wr, numpipes, 1, NULL,
                                         mac_size) < 1) {
                if (!ossl_statem_in_error(s)) {
                    SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                }
                goto err;
            }
        }
    }

    for (j = 0; j < numpipes; j++) {
        size_t origlen;

        thispkt = &pkt[j];
        thiswr = &wr[j];

        if (using_ktls)
            goto mac_done;

        /* Allocate bytes for the encryption overhead */
        if (!WPACKET_get_length(thispkt, &origlen)
                   /* Encryption should never shrink the data! */
                || origlen > thiswr->length
                || (thiswr->length > origlen
                    && !WPACKET_allocate_bytes(thispkt,
                                               thiswr->length - origlen,
                                               NULL))) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            goto err;
        }
        if (SSL_WRITE_ETM(s) && mac_size != 0) {
            unsigned char *mac;

            if (!WPACKET_allocate_bytes(thispkt, mac_size, &mac)
                    || !ssl->method->ssl3_enc->mac(s, thiswr, mac, 1)) {
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                goto err;
            }
            SSL3_RECORD_add_length(thiswr, mac_size);
        }

        if (!WPACKET_get_length(thispkt, &len)
                || !WPACKET_close(thispkt)) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            goto err;
        }

        if (s->msg_callback) {
            recordstart = WPACKET_get_curr(thispkt) - len
                          - SSL3_RT_HEADER_LENGTH;
            s->msg_callback(1, thiswr->rec_version, SSL3_RT_HEADER, recordstart,
                            SSL3_RT_HEADER_LENGTH, ssl,
                            s->msg_callback_arg);

            if (SSL_CONNECTION_TREAT_AS_TLS13(s) && s->enc_write_ctx != NULL) {
                unsigned char ctype = type;

                s->msg_callback(1, thiswr->rec_version, SSL3_RT_INNER_CONTENT_TYPE,
                                &ctype, 1, ssl, s->msg_callback_arg);
            }
        }

        if (!WPACKET_finish(thispkt)) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            goto err;
        }

        /* header is added by the kernel when using offload */
        SSL3_RECORD_add_length(thiswr, SSL3_RT_HEADER_LENGTH);

        if (create_empty_fragment) {
            /*
             * we are in a recursive call; just return the length, don't write
             * out anything here
             */
            if (j > 0) {
                /* We should never be pipelining an empty fragment!! */
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                goto err;
            }
            *written = SSL3_RECORD_get_length(thiswr);
            return 1;
        }

 mac_done:
        /*
         * we should now have thiswr->data pointing to the encrypted data, which
         * is thiswr->length long
         */
        SSL3_RECORD_set_type(thiswr, type); /* not needed but helps for
                                             * debugging */

        /* now let's set up wb */
        SSL3_BUFFER_set_left(&s->rlayer.wbuf[j],
                             prefix_len + SSL3_RECORD_get_length(thiswr));
    }

    /*
     * memorize arguments so that ssl3_write_pending can detect bad write
     * retries later
     */
    s->rlayer.wpend_tot = totlen;
    s->rlayer.wpend_buf = buf;
    s->rlayer.wpend_type = type;
    s->rlayer.wpend_ret = totlen;

    /* we now just need to write the buffer */
    return ssl3_write_pending(s, type, buf, totlen, written);
 err:
    for (j = 0; j < wpinited; j++)
        WPACKET_cleanup(&pkt[j]);
    return -1;
}

/* if SSL3_BUFFER_get_left() != 0, we need to call this
 *
 * Return values are as per SSL_write()
 */
int ssl3_write_pending(SSL_CONNECTION *s, int type, const unsigned char *buf,
                       size_t len, size_t *written)
{
    int i;
    SSL3_BUFFER *wb = s->rlayer.wbuf;
    size_t currbuf = 0;
    size_t tmpwrit = 0;

    if ((s->rlayer.wpend_tot > len)
        || (!(s->mode & SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER)
            && (s->rlayer.wpend_buf != buf))
        || (s->rlayer.wpend_type != type)) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_R_BAD_WRITE_RETRY);
        return -1;
    }

    for (;;) {
        /* Loop until we find a buffer we haven't written out yet */
        if (SSL3_BUFFER_get_left(&wb[currbuf]) == 0
            && currbuf < s->rlayer.numwpipes - 1) {
            currbuf++;
            continue;
        }
        clear_sys_error();
        if (s->wbio != NULL) {
            s->rwstate = SSL_WRITING;

            /*
             * To prevent coalescing of control and data messages,
             * such as in buffer_write, we flush the BIO
             */
            if (BIO_get_ktls_send(s->wbio) && type != SSL3_RT_APPLICATION_DATA) {
                i = BIO_flush(s->wbio);
                if (i <= 0)
                    return i;
                BIO_set_ktls_ctrl_msg(s->wbio, type);
            }
            i = BIO_write(s->wbio, (char *)
                          &(SSL3_BUFFER_get_buf(&wb[currbuf])
                            [SSL3_BUFFER_get_offset(&wb[currbuf])]),
                          (unsigned int)SSL3_BUFFER_get_left(&wb[currbuf]));
            if (i >= 0)
                tmpwrit = i;
        } else {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_R_BIO_NOT_SET);
            i = -1;
        }

        /*
         * When an empty fragment is sent on a connection using KTLS,
         * it is sent as a write of zero bytes.  If this zero byte
         * write succeeds, i will be 0 rather than a non-zero value.
         * Treat i == 0 as success rather than an error for zero byte
         * writes to permit this case.
         */
        if (i >= 0 && tmpwrit == SSL3_BUFFER_get_left(&wb[currbuf])) {
            SSL3_BUFFER_set_left(&wb[currbuf], 0);
            SSL3_BUFFER_add_offset(&wb[currbuf], tmpwrit);
            if (currbuf + 1 < s->rlayer.numwpipes)
                continue;
            s->rwstate = SSL_NOTHING;
            *written = s->rlayer.wpend_ret;
            return 1;
        } else if (i <= 0) {
            if (SSL_CONNECTION_IS_DTLS(s)) {
                /*
                 * For DTLS, just drop it. That's kind of the whole point in
                 * using a datagram service
                 */
                SSL3_BUFFER_set_left(&wb[currbuf], 0);
            }
            return i;
        }
        SSL3_BUFFER_add_offset(&wb[currbuf], tmpwrit);
        SSL3_BUFFER_sub_left(&wb[currbuf], tmpwrit);
    }
}

int ossl_tls_handle_rlayer_return(SSL_CONNECTION *s, int ret, char *file,
                                  int line)
{
    SSL *ssl = SSL_CONNECTION_GET_SSL(s);

    if (ret == OSSL_RECORD_RETURN_RETRY) {
        s->rwstate = SSL_READING;
        ret = -1;
    } else {
        s->rwstate = SSL_NOTHING;
        if (ret == OSSL_RECORD_RETURN_EOF) {
            if (s->options & SSL_OP_IGNORE_UNEXPECTED_EOF) {
                SSL_set_shutdown(ssl, SSL_RECEIVED_SHUTDOWN);
                s->s3.warn_alert = SSL_AD_CLOSE_NOTIFY;
            } else {
                ERR_new();
                ERR_set_debug(file, line, 0);
                ossl_statem_fatal(s, SSL_AD_DECODE_ERROR,
                                  SSL_R_UNEXPECTED_EOF_WHILE_READING, NULL);
            }
        } else if (ret == OSSL_RECORD_RETURN_FATAL) {
            int al = s->rlayer.rrlmethod->get_alert_code(s->rlayer.rrl);

            if (al != SSL_AD_NO_ALERT) {
                ERR_new();
                ERR_set_debug(file, line, 0);
                ossl_statem_fatal(s, al, SSL_R_RECORD_LAYER_FAILURE, NULL);
            }
            /*
             * else some failure but there is no alert code. We don't log an
             * error for this. The record layer should have logged an error
             * already or, if not, its due to some sys call error which will be
             * reported via SSL_ERROR_SYSCALL and errno.
             */
        }
        /*
         * The record layer distinguishes the cases of EOF, non-fatal
         * err and retry. Upper layers do not.
         * If we got a retry or success then *ret is already correct,
         * otherwise we need to convert the return value.
         */
        if (ret == OSSL_RECORD_RETURN_NON_FATAL_ERR || ret == OSSL_RECORD_RETURN_EOF)
            ret = 0;
        else if (ret < OSSL_RECORD_RETURN_NON_FATAL_ERR)
            ret = -1;
    }

    return ret;
}

void ssl_release_record(SSL_CONNECTION *s, TLS_RECORD *rr)
{
    if (rr->rechandle != NULL) {
        /* The record layer allocated the buffers for this record */
        s->rlayer.rrlmethod->release_record(s->rlayer.rrl, rr->rechandle);
    } else {
        /* We allocated the buffers for this record (only happens with DTLS) */
        OPENSSL_free(rr->data);
    }
    s->rlayer.curr_rec++;
}

/*-
 * Return up to 'len' payload bytes received in 'type' records.
 * 'type' is one of the following:
 *
 *   -  SSL3_RT_HANDSHAKE (when ssl3_get_message calls us)
 *   -  SSL3_RT_APPLICATION_DATA (when ssl3_read calls us)
 *   -  0 (during a shutdown, no data has to be returned)
 *
 * If we don't have stored data to work from, read a SSL/TLS record first
 * (possibly multiple records if we still don't have anything to return).
 *
 * This function must handle any surprises the peer may have for us, such as
 * Alert records (e.g. close_notify) or renegotiation requests. ChangeCipherSpec
 * messages are treated as if they were handshake messages *if* the |recvd_type|
 * argument is non NULL.
 * Also if record payloads contain fragments too small to process, we store
 * them until there is enough for the respective protocol (the record protocol
 * may use arbitrary fragmentation and even interleaving):
 *     Change cipher spec protocol
 *             just 1 byte needed, no need for keeping anything stored
 *     Alert protocol
 *             2 bytes needed (AlertLevel, AlertDescription)
 *     Handshake protocol
 *             4 bytes needed (HandshakeType, uint24 length) -- we just have
 *             to detect unexpected Client Hello and Hello Request messages
 *             here, anything else is handled by higher layers
 *     Application data protocol
 *             none of our business
 */
int ssl3_read_bytes(SSL *ssl, int type, int *recvd_type, unsigned char *buf,
                    size_t len, int peek, size_t *readbytes)
{
    int i, j, ret;
    size_t n, curr_rec, totalbytes;
    TLS_RECORD *rr;
    void (*cb) (const SSL *ssl, int type2, int val) = NULL;
    int is_tls13;
    SSL_CONNECTION *s = SSL_CONNECTION_FROM_SSL_ONLY(ssl);

    is_tls13 = SSL_CONNECTION_IS_TLS13(s);

    if ((type != 0
            && (type != SSL3_RT_APPLICATION_DATA)
            && (type != SSL3_RT_HANDSHAKE))
        || (peek && (type != SSL3_RT_APPLICATION_DATA))) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
        return -1;
    }

    if ((type == SSL3_RT_HANDSHAKE) && (s->rlayer.handshake_fragment_len > 0))
        /* (partially) satisfy request from storage */
    {
        unsigned char *src = s->rlayer.handshake_fragment;
        unsigned char *dst = buf;
        unsigned int k;

        /* peek == 0 */
        n = 0;
        while ((len > 0) && (s->rlayer.handshake_fragment_len > 0)) {
            *dst++ = *src++;
            len--;
            s->rlayer.handshake_fragment_len--;
            n++;
        }
        /* move any remaining fragment bytes: */
        for (k = 0; k < s->rlayer.handshake_fragment_len; k++)
            s->rlayer.handshake_fragment[k] = *src++;

        if (recvd_type != NULL)
            *recvd_type = SSL3_RT_HANDSHAKE;

        *readbytes = n;
        return 1;
    }

    /*
     * Now s->rlayer.handshake_fragment_len == 0 if type == SSL3_RT_HANDSHAKE.
     */

    if (!ossl_statem_get_in_handshake(s) && SSL_in_init(ssl)) {
        /* type == SSL3_RT_APPLICATION_DATA */
        i = s->handshake_func(ssl);
        /* SSLfatal() already called */
        if (i < 0)
            return i;
        if (i == 0)
            return -1;
    }
 start:
    s->rwstate = SSL_NOTHING;

    /*-
     * For each record 'i' up to |num_recs]
     * rr[i].type     - is the type of record
     * rr[i].data,    - data
     * rr[i].off,     - offset into 'data' for next read
     * rr[i].length,  - number of bytes.
     */
    /* get new records if necessary */
    if (s->rlayer.curr_rec >= s->rlayer.num_recs) {
        s->rlayer.curr_rec = s->rlayer.num_recs = 0;
        do {
            rr = &s->rlayer.tlsrecs[s->rlayer.num_recs];

            ret = HANDLE_RLAYER_RETURN(s,
                    s->rlayer.rrlmethod->read_record(s->rlayer.rrl,
                                                     &rr->rechandle,
                                                     &rr->version, &rr->type,
                                                     &rr->data, &rr->length,
                                                     NULL, NULL));
            if (ret <= 0) {
                /* SSLfatal() already called if appropriate */
                return ret;
            }
            rr->off = 0;
            s->rlayer.num_recs++;
        } while (s->rlayer.rrlmethod->processed_read_pending(s->rlayer.rrl)
                 && s->rlayer.num_recs < SSL_MAX_PIPELINES);
    }
    rr = &s->rlayer.tlsrecs[s->rlayer.curr_rec];

    if (s->rlayer.handshake_fragment_len > 0
            && rr->type != SSL3_RT_HANDSHAKE
            && SSL_CONNECTION_IS_TLS13(s)) {
        SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE,
                 SSL_R_MIXED_HANDSHAKE_AND_NON_HANDSHAKE_DATA);
        return -1;
    }

    /*
     * Reset the count of consecutive warning alerts if we've got a non-empty
     * record that isn't an alert.
     */
    if (rr->type != SSL3_RT_ALERT && rr->length != 0)
        s->rlayer.alert_count = 0;

    /* we now have a packet which can be read and processed */

    if (s->s3.change_cipher_spec /* set when we receive ChangeCipherSpec,
                                  * reset by ssl3_get_finished */
        && (rr->type != SSL3_RT_HANDSHAKE)) {
        SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE,
                 SSL_R_DATA_BETWEEN_CCS_AND_FINISHED);
        return -1;
    }

    /*
     * If the other end has shut down, throw anything we read away (even in
     * 'peek' mode)
     */
    if (s->shutdown & SSL_RECEIVED_SHUTDOWN) {
        s->rlayer.curr_rec++;
        s->rwstate = SSL_NOTHING;
        return 0;
    }

    if (type == rr->type
        || (rr->type == SSL3_RT_CHANGE_CIPHER_SPEC
            && type == SSL3_RT_HANDSHAKE && recvd_type != NULL
            && !is_tls13)) {
        /*
         * SSL3_RT_APPLICATION_DATA or
         * SSL3_RT_HANDSHAKE or
         * SSL3_RT_CHANGE_CIPHER_SPEC
         */
        /*
         * make sure that we are not getting application data when we are
         * doing a handshake for the first time
         */
        if (SSL_in_init(ssl) && type == SSL3_RT_APPLICATION_DATA
            && s->enc_read_ctx == NULL) {
            SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE, SSL_R_APP_DATA_IN_HANDSHAKE);
            return -1;
        }

        if (type == SSL3_RT_HANDSHAKE
            && rr->type == SSL3_RT_CHANGE_CIPHER_SPEC
            && s->rlayer.handshake_fragment_len > 0) {
            SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE, SSL_R_CCS_RECEIVED_EARLY);
            return -1;
        }

        if (recvd_type != NULL)
            *recvd_type = rr->type;

        if (len == 0) {
            /*
             * Skip a zero length record. This ensures multiple calls to
             * SSL_read() with a zero length buffer will eventually cause
             * SSL_pending() to report data as being available.
             */
            if (rr->length == 0)
                ssl_release_record(s, rr);

            return 0;
        }

        totalbytes = 0;
        curr_rec = s->rlayer.curr_rec;
        do {
            if (len - totalbytes > rr->length)
                n = rr->length;
            else
                n = len - totalbytes;

            memcpy(buf, &(rr->data[rr->off]), n);
            buf += n;
            if (peek) {
                /* Mark any zero length record as consumed CVE-2016-6305 */
                if (rr->length == 0)
                    ssl_release_record(s, rr);
            } else {
                if (s->options & SSL_OP_CLEANSE_PLAINTEXT)
                    OPENSSL_cleanse(&(rr->data[rr->off]), n);
                rr->length -= n;
                rr->off += n;
                if (rr->length == 0)
                    ssl_release_record(s, rr);
            }
            if (rr->length == 0
                || (peek && n == rr->length)) {
                rr++;
                curr_rec++;
            }
            totalbytes += n;
        } while (type == SSL3_RT_APPLICATION_DATA
                    && curr_rec < s->rlayer.num_recs
                    && totalbytes < len);
        if (totalbytes == 0) {
            /* We must have read empty records. Get more data */
            goto start;
        }
        *readbytes = totalbytes;
        return 1;
    }

    /*
     * If we get here, then type != rr->type; if we have a handshake message,
     * then it was unexpected (Hello Request or Client Hello) or invalid (we
     * were actually expecting a CCS).
     */

    /*
     * Lets just double check that we've not got an SSLv2 record
     */
    if (rr->version == SSL2_VERSION) {
        /*
         * Should never happen. ssl3_get_record() should only give us an SSLv2
         * record back if this is the first packet and we are looking for an
         * initial ClientHello. Therefore |type| should always be equal to
         * |rr->type|. If not then something has gone horribly wrong
         */
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
        return -1;
    }

    if (ssl->method->version == TLS_ANY_VERSION
        && (s->server || rr->type != SSL3_RT_ALERT)) {
        /*
         * If we've got this far and still haven't decided on what version
         * we're using then this must be a client side alert we're dealing
         * with. We shouldn't be receiving anything other than a ClientHello
         * if we are a server.
         */
        s->version = rr->version;
        SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE, SSL_R_UNEXPECTED_MESSAGE);
        return -1;
    }

    /*-
     * s->rlayer.handshake_fragment_len == 4  iff  rr->type == SSL3_RT_HANDSHAKE;
     * (Possibly rr is 'empty' now, i.e. rr->length may be 0.)
     */

    if (rr->type == SSL3_RT_ALERT) {
        unsigned int alert_level, alert_descr;
        unsigned char *alert_bytes = rr->data
                                     + rr->off;
        PACKET alert;

        if (!PACKET_buf_init(&alert, alert_bytes, rr->length)
                || !PACKET_get_1(&alert, &alert_level)
                || !PACKET_get_1(&alert, &alert_descr)
                || PACKET_remaining(&alert) != 0) {
            SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE, SSL_R_INVALID_ALERT);
            return -1;
        }

        if (s->msg_callback)
            s->msg_callback(0, s->version, SSL3_RT_ALERT, alert_bytes, 2, ssl,
                            s->msg_callback_arg);

        if (s->info_callback != NULL)
            cb = s->info_callback;
        else if (ssl->ctx->info_callback != NULL)
            cb = ssl->ctx->info_callback;

        if (cb != NULL) {
            j = (alert_level << 8) | alert_descr;
            cb(ssl, SSL_CB_READ_ALERT, j);
        }

        if ((!is_tls13 && alert_level == SSL3_AL_WARNING)
                || (is_tls13 && alert_descr == SSL_AD_USER_CANCELLED)) {
            s->s3.warn_alert = alert_descr;
            ssl_release_record(s, rr);

            s->rlayer.alert_count++;
            if (s->rlayer.alert_count == MAX_WARN_ALERT_COUNT) {
                SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE,
                         SSL_R_TOO_MANY_WARN_ALERTS);
                return -1;
            }
        }

        /*
         * Apart from close_notify the only other warning alert in TLSv1.3
         * is user_cancelled - which we just ignore.
         */
        if (is_tls13 && alert_descr == SSL_AD_USER_CANCELLED) {
            goto start;
        } else if (alert_descr == SSL_AD_CLOSE_NOTIFY
                && (is_tls13 || alert_level == SSL3_AL_WARNING)) {
            s->shutdown |= SSL_RECEIVED_SHUTDOWN;
            return 0;
        } else if (alert_level == SSL3_AL_FATAL || is_tls13) {
            s->rwstate = SSL_NOTHING;
            s->s3.fatal_alert = alert_descr;
            SSLfatal_data(s, SSL_AD_NO_ALERT,
                          SSL_AD_REASON_OFFSET + alert_descr,
                          "SSL alert number %d", alert_descr);
            s->shutdown |= SSL_RECEIVED_SHUTDOWN;
            ssl_release_record(s, rr);
            SSL_CTX_remove_session(s->session_ctx, s->session);
            return 0;
        } else if (alert_descr == SSL_AD_NO_RENEGOTIATION) {
            /*
             * This is a warning but we receive it if we requested
             * renegotiation and the peer denied it. Terminate with a fatal
             * alert because if application tried to renegotiate it
             * presumably had a good reason and expects it to succeed. In
             * future we might have a renegotiation where we don't care if
             * the peer refused it where we carry on.
             */
            SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE, SSL_R_NO_RENEGOTIATION);
            return -1;
        } else if (alert_level == SSL3_AL_WARNING) {
            /* We ignore any other warning alert in TLSv1.2 and below */
            goto start;
        }

        SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER, SSL_R_UNKNOWN_ALERT_TYPE);
        return -1;
    }

    if ((s->shutdown & SSL_SENT_SHUTDOWN) != 0) {
        if (rr->type == SSL3_RT_HANDSHAKE) {
            BIO *rbio;

            /*
             * We ignore any handshake messages sent to us unless they are
             * TLSv1.3 in which case we want to process them. For all other
             * handshake messages we can't do anything reasonable with them
             * because we are unable to write any response due to having already
             * sent close_notify.
             */
            if (!SSL_CONNECTION_IS_TLS13(s)) {
                ssl_release_record(s, rr);

                if ((s->mode & SSL_MODE_AUTO_RETRY) != 0)
                    goto start;

                s->rwstate = SSL_READING;
                rbio = SSL_get_rbio(ssl);
                BIO_clear_retry_flags(rbio);
                BIO_set_retry_read(rbio);
                return -1;
            }
        } else {
            /*
             * The peer is continuing to send application data, but we have
             * already sent close_notify. If this was expected we should have
             * been called via SSL_read() and this would have been handled
             * above.
             * No alert sent because we already sent close_notify
             */
            ssl_release_record(s, rr);
            SSLfatal(s, SSL_AD_NO_ALERT,
                     SSL_R_APPLICATION_DATA_AFTER_CLOSE_NOTIFY);
            return -1;
        }
    }

    /*
     * For handshake data we have 'fragment' storage, so fill that so that we
     * can process the header at a fixed place. This is done after the
     * "SHUTDOWN" code above to avoid filling the fragment storage with data
     * that we're just going to discard.
     */
    if (rr->type == SSL3_RT_HANDSHAKE) {
        size_t dest_maxlen = sizeof(s->rlayer.handshake_fragment);
        unsigned char *dest = s->rlayer.handshake_fragment;
        size_t *dest_len = &s->rlayer.handshake_fragment_len;

        n = dest_maxlen - *dest_len; /* available space in 'dest' */
        if (rr->length < n)
            n = rr->length; /* available bytes */

        /* now move 'n' bytes: */
        memcpy(dest + *dest_len, rr->data + rr->off, n);
        rr->off += n;
        rr->length -= n;
        *dest_len += n;
        if (rr->length == 0)
            ssl_release_record(s, rr);

        if (*dest_len < dest_maxlen)
            goto start;     /* fragment was too small */
    }

    if (rr->type == SSL3_RT_CHANGE_CIPHER_SPEC) {
        SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE, SSL_R_CCS_RECEIVED_EARLY);
        return -1;
    }

    /*
     * Unexpected handshake message (ClientHello, NewSessionTicket (TLS1.3) or
     * protocol violation)
     */
    if ((s->rlayer.handshake_fragment_len >= 4)
            && !ossl_statem_get_in_handshake(s)) {
        int ined = (s->early_data_state == SSL_EARLY_DATA_READING);

        /* We found handshake data, so we're going back into init */
        ossl_statem_set_in_init(s, 1);

        i = s->handshake_func(ssl);
        /* SSLfatal() already called if appropriate */
        if (i < 0)
            return i;
        if (i == 0) {
            return -1;
        }

        /*
         * If we were actually trying to read early data and we found a
         * handshake message, then we don't want to continue to try and read
         * the application data any more. It won't be "early" now.
         */
        if (ined)
            return -1;

        if (!(s->mode & SSL_MODE_AUTO_RETRY)) {
            if (!RECORD_LAYER_read_pending(&s->rlayer)) {
                BIO *bio;
                /*
                 * In the case where we try to read application data, but we
                 * trigger an SSL handshake, we return -1 with the retry
                 * option set.  Otherwise renegotiation may cause nasty
                 * problems in the blocking world
                 */
                s->rwstate = SSL_READING;
                bio = SSL_get_rbio(ssl);
                BIO_clear_retry_flags(bio);
                BIO_set_retry_read(bio);
                return -1;
            }
        }
        goto start;
    }

    switch (rr->type) {
    default:
        /*
         * TLS 1.0 and 1.1 say you SHOULD ignore unrecognised record types, but
         * TLS 1.2 says you MUST send an unexpected message alert. We use the
         * TLS 1.2 behaviour for all protocol versions to prevent issues where
         * no progress is being made and the peer continually sends unrecognised
         * record types, using up resources processing them.
         */
        SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE, SSL_R_UNEXPECTED_RECORD);
        return -1;
    case SSL3_RT_CHANGE_CIPHER_SPEC:
    case SSL3_RT_ALERT:
    case SSL3_RT_HANDSHAKE:
        /*
         * we already handled all of these, with the possible exception of
         * SSL3_RT_HANDSHAKE when ossl_statem_get_in_handshake(s) is true, but
         * that should not happen when type != rr->type
         */
        SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE, ERR_R_INTERNAL_ERROR);
        return -1;
    case SSL3_RT_APPLICATION_DATA:
        /*
         * At this point, we were expecting handshake data, but have
         * application data.  If the library was running inside ssl3_read()
         * (i.e. in_read_app_data is set) and it makes sense to read
         * application data at this point (session renegotiation not yet
         * started), we will indulge it.
         */
        if (ossl_statem_app_data_allowed(s)) {
            s->s3.in_read_app_data = 2;
            return -1;
        } else if (ossl_statem_skip_early_data(s)) {
            /*
             * This can happen after a client sends a CH followed by early_data,
             * but the server responds with a HelloRetryRequest. The server
             * reads the next record from the client expecting to find a
             * plaintext ClientHello but gets a record which appears to be
             * application data. The trial decrypt "works" because null
             * decryption was applied. We just skip it and move on to the next
             * record.
             */
            if (!ossl_early_data_count_ok(s, rr->length,
                                          EARLY_DATA_CIPHERTEXT_OVERHEAD, 0)) {
                /* SSLfatal() already called */
                return -1;
            }
            ssl_release_record(s, rr);
            goto start;
        } else {
            SSLfatal(s, SSL_AD_UNEXPECTED_MESSAGE, SSL_R_UNEXPECTED_RECORD);
            return -1;
        }
    }
}

void ssl3_record_sequence_update(unsigned char *seq)
{
    int i;

    for (i = 7; i >= 0; i--) {
        ++seq[i];
        if (seq[i] != 0)
            break;
    }
}

/*
 * Returns true if the current rrec was sent in SSLv2 backwards compatible
 * format and false otherwise.
 */
int RECORD_LAYER_is_sslv2_record(RECORD_LAYER *rl)
{
    if (SSL_CONNECTION_IS_DTLS(rl->s))
        return 0;
    return rl->tlsrecs[0].version == SSL2_VERSION;
}

static OSSL_FUNC_rlayer_msg_callback_fn rlayer_msg_callback_wrapper;
static void rlayer_msg_callback_wrapper(int write_p, int version,
                                        int content_type, const void *buf,
                                        size_t len, void *cbarg)
{
    SSL_CONNECTION *s = cbarg;
    SSL *ssl = SSL_CONNECTION_GET_SSL(s);

    if (s->msg_callback != NULL)
        s->msg_callback(write_p, version, content_type, buf, len, ssl,
                        s->msg_callback_arg);
}

static OSSL_FUNC_rlayer_security_fn rlayer_security_wrapper;
static int rlayer_security_wrapper(void *cbarg, int op, int bits, int nid,
                                   void *other)
{
    SSL_CONNECTION *s = cbarg;

    return ssl_security(s, op, bits, nid, other);
}

static const OSSL_DISPATCH rlayer_dispatch[] = {
    { OSSL_FUNC_RLAYER_SKIP_EARLY_DATA, (void (*)(void))ossl_statem_skip_early_data },
    { OSSL_FUNC_RLAYER_MSG_CALLBACK, (void (*)(void))rlayer_msg_callback_wrapper },
    { OSSL_FUNC_RLAYER_SECURITY, (void (*)(void))rlayer_security_wrapper },
    { 0, NULL }
};

static const OSSL_RECORD_METHOD *ssl_select_next_record_layer(SSL_CONNECTION *s,
                                                              int level)
{

    if (level == OSSL_RECORD_PROTECTION_LEVEL_NONE) {
        if (SSL_CONNECTION_IS_DTLS(s))
            return &ossl_dtls_record_method;

        return &ossl_tls_record_method;
    }

#ifndef OPENSSL_NO_KTLS
    /* KTLS does not support renegotiation */
    if (level == OSSL_RECORD_PROTECTION_LEVEL_APPLICATION
            && (s->options & SSL_OP_ENABLE_KTLS) != 0
            && (SSL_CONNECTION_IS_TLS13(s) || SSL_IS_FIRST_HANDSHAKE(s)))
        return &ossl_ktls_record_method;
#endif

    /* Default to the current OSSL_RECORD_METHOD */
    return s->rlayer.rrlmethod;
}

static int ssl_post_record_layer_select(SSL_CONNECTION *s)
{
#ifndef OPENSSL_NO_KTLS
    SSL *ssl = SSL_CONNECTION_GET_SSL(s);

    if (s->rlayer.rrlmethod == &ossl_ktls_record_method) {
        /* KTLS does not support renegotiation so disallow it */
        SSL_set_options(ssl, SSL_OP_NO_RENEGOTIATION);
    }
#endif
    if (SSL_IS_FIRST_HANDSHAKE(s) && s->rlayer.rrlmethod->set_first_handshake != NULL)
        s->rlayer.rrlmethod->set_first_handshake(s->rlayer.rrl, 1);

    if (s->max_pipelines != 0 && s->rlayer.rrlmethod->set_max_pipelines != NULL)
        s->rlayer.rrlmethod->set_max_pipelines(s->rlayer.rrl, s->max_pipelines);

    return 1;
}

int ssl_set_new_record_layer(SSL_CONNECTION *s, int version,
                             int direction, int level,
                             unsigned char *key, size_t keylen,
                             unsigned char *iv,  size_t ivlen,
                             unsigned char *mackey, size_t mackeylen,
                             const EVP_CIPHER *ciph, size_t taglen,
                             int mactype, const EVP_MD *md,
                             const SSL_COMP *comp)
{
    OSSL_PARAM options[5], *opts = options;
    OSSL_PARAM settings[6], *set =  settings;
    const OSSL_RECORD_METHOD *origmeth = s->rlayer.rrlmethod;
    SSL_CTX *sctx = SSL_CONNECTION_GET_CTX(s);
    const OSSL_RECORD_METHOD *meth;
    int use_etm, stream_mac = 0, tlstree = 0;
    unsigned int maxfrag = SSL3_RT_MAX_PLAIN_LENGTH;
    int use_early_data = 0;
    uint32_t max_early_data;

    meth = ssl_select_next_record_layer(s, level);

    if (s->rlayer.rrlmethod != NULL && !s->rlayer.rrlmethod->free(s->rlayer.rrl)) {
        ERR_raise(ERR_LIB_SSL, ERR_R_INTERNAL_ERROR);
        return 0;
    }

    s->rlayer.rrl = NULL;
    if (meth != NULL)
        s->rlayer.rrlmethod = meth;

    if (!ossl_assert(s->rlayer.rrlmethod != NULL)) {
        ERR_raise(ERR_LIB_SSL, ERR_R_INTERNAL_ERROR);
        return 0;
    }

    /* Parameters that *may* be supported by a record layer if passed */
    *opts++ = OSSL_PARAM_construct_uint64(OSSL_LIBSSL_RECORD_LAYER_PARAM_OPTIONS,
                                          &s->options);
    *opts++ = OSSL_PARAM_construct_uint32(OSSL_LIBSSL_RECORD_LAYER_PARAM_MODE,
                                          &s->mode);
    *opts++ = OSSL_PARAM_construct_size_t(OSSL_LIBSSL_RECORD_LAYER_READ_BUFFER_LEN,
                                          &s->rlayer.default_read_buf_len);
    *opts++ = OSSL_PARAM_construct_int(OSSL_LIBSSL_RECORD_LAYER_PARAM_READ_AHEAD,
                                       &s->rlayer.read_ahead);
    *opts = OSSL_PARAM_construct_end();

    /* Parameters that *must* be supported by a record layer if passed */
    if (direction == OSSL_RECORD_DIRECTION_READ) {
        use_etm = SSL_READ_ETM(s) ? 1 : 0;
        if ((s->mac_flags & SSL_MAC_FLAG_READ_MAC_STREAM) != 0)
            stream_mac = 1;

        if ((s->mac_flags & SSL_MAC_FLAG_READ_MAC_TLSTREE) != 0)
            tlstree = 1;
    } else {
        use_etm = SSL_WRITE_ETM(s) ? 1 : 0;
        if ((s->mac_flags & SSL_MAC_FLAG_WRITE_MAC_STREAM) != 0)
            stream_mac = 1;

        if ((s->mac_flags & SSL_MAC_FLAG_WRITE_MAC_TLSTREE) != 0)
            tlstree = 1;
    }

    if (use_etm)
        *set++ = OSSL_PARAM_construct_int(OSSL_LIBSSL_RECORD_LAYER_PARAM_USE_ETM,
                                          &use_etm);

    if (stream_mac)
        *set++ = OSSL_PARAM_construct_int(OSSL_LIBSSL_RECORD_LAYER_PARAM_STREAM_MAC,
                                          &stream_mac);

    if (tlstree)
        *set++ = OSSL_PARAM_construct_int(OSSL_LIBSSL_RECORD_LAYER_PARAM_TLSTREE,
                                          &tlstree);

    if (s->session != NULL && USE_MAX_FRAGMENT_LENGTH_EXT(s->session))
        maxfrag = GET_MAX_FRAGMENT_LENGTH(s->session);

    if (maxfrag != SSL3_RT_MAX_PLAIN_LENGTH)
        *set++ = OSSL_PARAM_construct_uint(OSSL_LIBSSL_RECORD_LAYER_PARAM_MAX_FRAG_LEN,
                                           &maxfrag);

    /*
     * The record layer must check the amount of early data sent or received
     * using the early keys. A server also needs to worry about rejected early
     * data that might arrive when the handshake keys are in force.
     */
    /* TODO(RECLAYER): Check this when doing the "write" record layer */
    if (s->server && direction == OSSL_RECORD_DIRECTION_READ) {
        use_early_data = (level == OSSL_RECORD_PROTECTION_LEVEL_EARLY
                          || level == OSSL_RECORD_PROTECTION_LEVEL_HANDSHAKE);
    } else if (!s->server && direction == OSSL_RECORD_DIRECTION_WRITE) {
        use_early_data = (level == OSSL_RECORD_PROTECTION_LEVEL_EARLY);
    }
    if (use_early_data) {
        max_early_data = ossl_get_max_early_data(s);

        if (max_early_data != 0)
            *set++ = OSSL_PARAM_construct_uint(OSSL_LIBSSL_RECORD_LAYER_PARAM_MAX_EARLY_DATA,
                                            &max_early_data);
    }

    *set = OSSL_PARAM_construct_end();

    for (;;) {
        int rlret;
        BIO *prev = s->rlayer.rrlnext;
        unsigned int epoch = 0;;

        if (SSL_CONNECTION_IS_DTLS(s)
                && level != OSSL_RECORD_PROTECTION_LEVEL_NONE)
            epoch =  DTLS_RECORD_LAYER_get_r_epoch(&s->rlayer) + 1; /* new epoch */

        if (SSL_CONNECTION_IS_DTLS(s))
            s->rlayer.rrlnext = BIO_new(BIO_s_dgram_mem());
        else
            s->rlayer.rrlnext = BIO_new(BIO_s_mem());

        if (s->rlayer.rrlnext == NULL) {
            BIO_free(prev);
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            return 0;
        }

        rlret = s->rlayer.rrlmethod->new_record_layer(sctx->libctx,
                                                      sctx->propq,
                                                      version, s->server,
                                                      direction, level, epoch,
                                                      key, keylen, iv, ivlen,
                                                      mackey, mackeylen, ciph,
                                                      taglen, mactype, md, comp,
                                                      prev, s->rbio,
                                                      s->rlayer.rrlnext, NULL,
                                                      NULL, settings, options,
                                                      rlayer_dispatch, s,
                                                      &s->rlayer.rrl);
        BIO_free(prev);
        switch (rlret) {
        case OSSL_RECORD_RETURN_FATAL:
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_R_RECORD_LAYER_FAILURE);
            return 0;

        case OSSL_RECORD_RETURN_NON_FATAL_ERR:
            if (s->rlayer.rrlmethod != origmeth && origmeth != NULL) {
                /*
                 * We tried a new record layer method, but it didn't work out,
                 * so we fallback to the original method and try again
                 */
                s->rlayer.rrlmethod = origmeth;
                continue;
            }
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_R_NO_SUITABLE_RECORD_LAYER);
            return 0;

        case OSSL_RECORD_RETURN_SUCCESS:
            break;

        default:
            /* Should not happen */
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            return 0;
        }
        break;
    }

    return ssl_post_record_layer_select(s);
}
