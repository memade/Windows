/*
 * Copyright 2022 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/rand.h>
#include "../../ssl_local.h"
#include "../record_local.h"
#include "recmethod_local.h"

static int tls1_set_crypto_state(OSSL_RECORD_LAYER *rl, int level,
                                 unsigned char *key, size_t keylen,
                                 unsigned char *iv, size_t ivlen,
                                 unsigned char *mackey, size_t mackeylen,
                                 const EVP_CIPHER *ciph,
                                 size_t taglen,
                                 int mactype,
                                 const EVP_MD *md,
                                 const SSL_COMP *comp)
{
    EVP_CIPHER_CTX *ciph_ctx;
    EVP_PKEY *mac_key;

    if (level != OSSL_RECORD_PROTECTION_LEVEL_APPLICATION)
        return OSSL_RECORD_RETURN_FATAL;

    if ((rl->enc_ctx = EVP_CIPHER_CTX_new()) == NULL) {
        RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_MALLOC_FAILURE);
        return OSSL_RECORD_RETURN_FATAL;
    }

    ciph_ctx = rl->enc_ctx;

    rl->md_ctx = EVP_MD_CTX_new();
    if (rl->md_ctx == NULL) {
        RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
        return OSSL_RECORD_RETURN_FATAL;
    }
#ifndef OPENSSL_NO_COMP
    if (comp != NULL) {
        rl->expand = COMP_CTX_new(comp->method);
        if (rl->expand == NULL) {
            ERR_raise(ERR_LIB_SSL, SSL_R_COMPRESSION_LIBRARY_ERROR);
            return OSSL_RECORD_RETURN_FATAL;
        }
    }
#endif

    /*
     * If we have an AEAD Cipher, then there is no separate MAC, so we can skip
     * setting up the MAC key.
     */
    if ((EVP_CIPHER_get_flags(ciph) & EVP_CIPH_FLAG_AEAD_CIPHER) == 0) {
        if (mactype == EVP_PKEY_HMAC) {
            mac_key = EVP_PKEY_new_raw_private_key_ex(rl->libctx, "HMAC",
                                                      rl->propq, mackey,
                                                      mackeylen);
        } else {
            /*
             * If its not HMAC then the only other types of MAC we support are
             * the GOST MACs, so we need to use the old style way of creating
             * a MAC key.
             */
            mac_key = EVP_PKEY_new_mac_key(mactype, NULL, mackey,
                                           (int)mackeylen);
        }
        if (mac_key == NULL
            || EVP_DigestSignInit_ex(rl->md_ctx, NULL, EVP_MD_get0_name(md),
                                     rl->libctx, rl->propq, mac_key,
                                     NULL) <= 0) {
            EVP_PKEY_free(mac_key);
            ERR_raise(ERR_LIB_SSL, ERR_R_INTERNAL_ERROR);
            return OSSL_RECORD_RETURN_FATAL;
        }
        EVP_PKEY_free(mac_key);
    }

    if (EVP_CIPHER_get_mode(ciph) == EVP_CIPH_GCM_MODE) {
        if (!EVP_DecryptInit_ex(ciph_ctx, ciph, NULL, key, NULL)
                || EVP_CIPHER_CTX_ctrl(ciph_ctx, EVP_CTRL_GCM_SET_IV_FIXED,
                                       (int)ivlen, iv) <= 0) {
            ERR_raise(ERR_LIB_SSL, ERR_R_INTERNAL_ERROR);
            return OSSL_RECORD_RETURN_FATAL;
        }
    } else if (EVP_CIPHER_get_mode(ciph) == EVP_CIPH_CCM_MODE) {
        if (!EVP_DecryptInit_ex(ciph_ctx, ciph, NULL, NULL, NULL)
                || EVP_CIPHER_CTX_ctrl(ciph_ctx, EVP_CTRL_AEAD_SET_IVLEN, 12,
                                       NULL) <= 0
                || EVP_CIPHER_CTX_ctrl(ciph_ctx, EVP_CTRL_AEAD_SET_TAG,
                                       (int)taglen, NULL) <= 0
                || EVP_CIPHER_CTX_ctrl(ciph_ctx, EVP_CTRL_CCM_SET_IV_FIXED,
                                       (int)ivlen, iv) <= 0
                || !EVP_DecryptInit_ex(ciph_ctx, NULL, NULL, key, NULL)) {
            ERR_raise(ERR_LIB_SSL, ERR_R_INTERNAL_ERROR);
            return OSSL_RECORD_RETURN_FATAL;
        }
    } else {
        if (!EVP_DecryptInit_ex(ciph_ctx, ciph, NULL, key, iv)) {
            ERR_raise(ERR_LIB_SSL, ERR_R_INTERNAL_ERROR);
            return OSSL_RECORD_RETURN_FATAL;
        }
    }
    /* Needed for "composite" AEADs, such as RC4-HMAC-MD5 */
    if ((EVP_CIPHER_get_flags(ciph) & EVP_CIPH_FLAG_AEAD_CIPHER) != 0
        && mackeylen != 0
        && EVP_CIPHER_CTX_ctrl(ciph_ctx, EVP_CTRL_AEAD_SET_MAC_KEY,
                               (int)mackeylen, mackey) <= 0) {
        ERR_raise(ERR_LIB_SSL, ERR_R_INTERNAL_ERROR);
        return OSSL_RECORD_RETURN_FATAL;
    }
    if (EVP_CIPHER_get0_provider(ciph) != NULL
            && !ossl_set_tls_provider_parameters(rl, ciph_ctx, ciph, md))
        return OSSL_RECORD_RETURN_FATAL;

    return OSSL_RECORD_RETURN_SUCCESS;
}

#define MAX_PADDING 256
/*-
 * tls1_cipher encrypts/decrypts |n_recs| in |recs|. Calls RLAYERfatal on
 * internal error, but not otherwise. It is the responsibility of the caller to
 * report a bad_record_mac - if appropriate (DTLS just drops the record).
 *
 * Returns:
 *    0: if the record is publicly invalid, or an internal error, or AEAD
 *       decryption failed, or Encrypt-then-mac decryption failed.
 *    1: Success or Mac-then-encrypt decryption failed (MAC will be randomised)
 */
static int tls1_cipher(OSSL_RECORD_LAYER *rl, SSL3_RECORD *recs, size_t n_recs,
                       int sending, SSL_MAC_BUF *macs, size_t macsize)
{
    EVP_CIPHER_CTX *ds;
    size_t reclen[SSL_MAX_PIPELINES];
    unsigned char buf[SSL_MAX_PIPELINES][EVP_AEAD_TLS1_AAD_LEN];
    int i, pad = 0, tmpr, provided;
    size_t bs, ctr, padnum, loop;
    unsigned char padval;
    const EVP_CIPHER *enc;

    if (n_recs == 0) {
        RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
        return 0;
    }

    if (EVP_MD_CTX_get0_md(rl->md_ctx)) {
        int n = EVP_MD_CTX_get_size(rl->md_ctx);

        if (!ossl_assert(n >= 0)) {
            RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            return 0;
        }
    }
    ds = rl->enc_ctx;
    if (!ossl_assert(rl->enc_ctx != NULL)) {
        RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
        return 0;
    }

    enc = EVP_CIPHER_CTX_get0_cipher(rl->enc_ctx);

    if (sending) {
        int ivlen;

        /* For TLSv1.1 and later explicit IV */
        if (RLAYER_USE_EXPLICIT_IV(rl)
            && EVP_CIPHER_get_mode(enc) == EVP_CIPH_CBC_MODE)
            ivlen = EVP_CIPHER_get_iv_length(enc);
        else
            ivlen = 0;
        if (ivlen > 1) {
            for (ctr = 0; ctr < n_recs; ctr++) {
                if (recs[ctr].data != recs[ctr].input) {
                    RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                    return 0;
                } else if (RAND_bytes_ex(rl->libctx, recs[ctr].input,
                                         ivlen, 0) <= 0) {
                    RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                    return 0;
                }
            }
        }
    }
    if (!ossl_assert(enc != NULL)) {
        RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
        return 0;
    }

    provided = (EVP_CIPHER_get0_provider(enc) != NULL);

    bs = EVP_CIPHER_get_block_size(EVP_CIPHER_CTX_get0_cipher(ds));

    if (n_recs > 1) {
        if ((EVP_CIPHER_get_flags(EVP_CIPHER_CTX_get0_cipher(ds))
                 & EVP_CIPH_FLAG_PIPELINE) == 0) {
            /*
             * We shouldn't have been called with pipeline data if the
             * cipher doesn't support pipelining
             */
            RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, SSL_R_PIPELINE_FAILURE);
            return 0;
        }
    }
    for (ctr = 0; ctr < n_recs; ctr++) {
        reclen[ctr] = recs[ctr].length;

        if ((EVP_CIPHER_get_flags(EVP_CIPHER_CTX_get0_cipher(ds))
                 & EVP_CIPH_FLAG_AEAD_CIPHER) != 0) {
            unsigned char *seq;

            seq = rl->sequence;

            if (rl->isdtls) {
                unsigned char dtlsseq[8], *p = dtlsseq;

                s2n(rl->epoch, p);
                memcpy(p, &seq[2], 6);
                memcpy(buf[ctr], dtlsseq, 8);
            } else {
                memcpy(buf[ctr], seq, 8);
                for (i = 7; i >= 0; i--) { /* increment */
                    ++seq[i];
                    if (seq[i] != 0)
                        break;
                }
            }

            buf[ctr][8] = recs[ctr].type;
            buf[ctr][9] = (unsigned char)(rl->version >> 8);
            buf[ctr][10] = (unsigned char)(rl->version);
            buf[ctr][11] = (unsigned char)(recs[ctr].length >> 8);
            buf[ctr][12] = (unsigned char)(recs[ctr].length & 0xff);
            pad = EVP_CIPHER_CTX_ctrl(ds, EVP_CTRL_AEAD_TLS1_AAD,
                                      EVP_AEAD_TLS1_AAD_LEN, buf[ctr]);
            if (pad <= 0) {
                RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                return 0;
            }

            if (sending) {
                reclen[ctr] += pad;
                recs[ctr].length += pad;
            }
        } else if ((bs != 1) && sending && !provided) {
            /*
             * We only do this for legacy ciphers. Provided ciphers add the
             * padding on the provider side.
             */
            padnum = bs - (reclen[ctr] % bs);

            /* Add weird padding of up to 256 bytes */

            if (padnum > MAX_PADDING) {
                RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
                return 0;
            }
            /* we need to add 'padnum' padding bytes of value padval */
            padval = (unsigned char)(padnum - 1);
            for (loop = reclen[ctr]; loop < reclen[ctr] + padnum; loop++)
                recs[ctr].input[loop] = padval;
            reclen[ctr] += padnum;
            recs[ctr].length += padnum;
        }

        if (!sending) {
            if (reclen[ctr] == 0 || reclen[ctr] % bs != 0) {
                /* Publicly invalid */
                return 0;
            }
        }
    }
    if (n_recs > 1) {
        unsigned char *data[SSL_MAX_PIPELINES];

        /* Set the output buffers */
        for (ctr = 0; ctr < n_recs; ctr++)
            data[ctr] = recs[ctr].data;

        if (EVP_CIPHER_CTX_ctrl(ds, EVP_CTRL_SET_PIPELINE_OUTPUT_BUFS,
                                (int)n_recs, data) <= 0) {
            RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, SSL_R_PIPELINE_FAILURE);
            return 0;
        }
        /* Set the input buffers */
        for (ctr = 0; ctr < n_recs; ctr++)
            data[ctr] = recs[ctr].input;

        if (EVP_CIPHER_CTX_ctrl(ds, EVP_CTRL_SET_PIPELINE_INPUT_BUFS,
                                (int)n_recs, data) <= 0
            || EVP_CIPHER_CTX_ctrl(ds, EVP_CTRL_SET_PIPELINE_INPUT_LENS,
                                   (int)n_recs, reclen) <= 0) {
            RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, SSL_R_PIPELINE_FAILURE);
            return 0;
        }
    }

    if (!rl->isdtls && rl->tlstree) {
        unsigned char *seq;
        int decrement_seq = 0;

        /*
         * When sending, seq is incremented after MAC calculation.
         * So if we are in ETM mode, we use seq 'as is' in the ctrl-function.
         * Otherwise we have to decrease it in the implementation
         */
        if (sending && !rl->use_etm)
            decrement_seq = 1;

        seq = rl->sequence;
        if (EVP_CIPHER_CTX_ctrl(ds, EVP_CTRL_TLSTREE, decrement_seq, seq) <= 0) {
            RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            return 0;
        }
    }

    if (provided) {
        int outlen;

        /* Provided cipher - we do not support pipelining on this path */
        if (n_recs > 1) {
            RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR, ERR_R_INTERNAL_ERROR);
            return 0;
        }

        if (!EVP_CipherUpdate(ds, recs[0].data, &outlen, recs[0].input,
                              (unsigned int)reclen[0]))
            return 0;
        recs[0].length = outlen;

        /*
         * The length returned from EVP_CipherUpdate above is the actual
         * payload length. We need to adjust the data/input ptr to skip over
         * any explicit IV
         */
        if (!sending) {
            if (EVP_CIPHER_get_mode(enc) == EVP_CIPH_GCM_MODE) {
                recs[0].data += EVP_GCM_TLS_EXPLICIT_IV_LEN;
                recs[0].input += EVP_GCM_TLS_EXPLICIT_IV_LEN;
            } else if (EVP_CIPHER_get_mode(enc) == EVP_CIPH_CCM_MODE) {
                recs[0].data += EVP_CCM_TLS_EXPLICIT_IV_LEN;
                recs[0].input += EVP_CCM_TLS_EXPLICIT_IV_LEN;
            } else if (bs != 1 && RLAYER_USE_EXPLICIT_IV(rl)) {
                recs[0].data += bs;
                recs[0].input += bs;
                recs[0].orig_len -= bs;
            }

            /* Now get a pointer to the MAC (if applicable) */
            if (macs != NULL) {
                OSSL_PARAM params[2], *p = params;

                /* Get the MAC */
                macs[0].alloced = 0;

                *p++ = OSSL_PARAM_construct_octet_ptr(OSSL_CIPHER_PARAM_TLS_MAC,
                                                      (void **)&macs[0].mac,
                                                      macsize);
                *p = OSSL_PARAM_construct_end();

                if (!EVP_CIPHER_CTX_get_params(ds, params)) {
                    /* Shouldn't normally happen */
                    RLAYERfatal(rl, SSL_AD_INTERNAL_ERROR,
                                ERR_R_INTERNAL_ERROR);
                    return 0;
                }
            }
        }
    } else {
        /* Legacy cipher */

        tmpr = EVP_Cipher(ds, recs[0].data, recs[0].input,
                          (unsigned int)reclen[0]);
        if ((EVP_CIPHER_get_flags(EVP_CIPHER_CTX_get0_cipher(ds))
                 & EVP_CIPH_FLAG_CUSTOM_CIPHER) != 0
            ? (tmpr < 0)
            : (tmpr == 0)) {
            /* AEAD can fail to verify MAC */
            return 0;
        }

        if (!sending) {
            for (ctr = 0; ctr < n_recs; ctr++) {
                /* Adjust the record to remove the explicit IV/MAC/Tag */
                if (EVP_CIPHER_get_mode(enc) == EVP_CIPH_GCM_MODE) {
                    recs[ctr].data += EVP_GCM_TLS_EXPLICIT_IV_LEN;
                    recs[ctr].input += EVP_GCM_TLS_EXPLICIT_IV_LEN;
                    recs[ctr].length -= EVP_GCM_TLS_EXPLICIT_IV_LEN;
                } else if (EVP_CIPHER_get_mode(enc) == EVP_CIPH_CCM_MODE) {
                    recs[ctr].data += EVP_CCM_TLS_EXPLICIT_IV_LEN;
                    recs[ctr].input += EVP_CCM_TLS_EXPLICIT_IV_LEN;
                    recs[ctr].length -= EVP_CCM_TLS_EXPLICIT_IV_LEN;
                } else if (bs != 1 && RLAYER_USE_EXPLICIT_IV(rl)) {
                    if (recs[ctr].length < bs)
                        return 0;
                    recs[ctr].data += bs;
                    recs[ctr].input += bs;
                    recs[ctr].length -= bs;
                    recs[ctr].orig_len -= bs;
                }

                /*
                 * If using Mac-then-encrypt, then this will succeed but
                 * with a random MAC if padding is invalid
                 */
                if (!tls1_cbc_remove_padding_and_mac(&recs[ctr].length,
                                        recs[ctr].orig_len,
                                        recs[ctr].data,
                                        (macs != NULL) ? &macs[ctr].mac : NULL,
                                        (macs != NULL) ? &macs[ctr].alloced
                                                       : NULL,
                                        bs,
                                        pad ? (size_t)pad : macsize,
                                        (EVP_CIPHER_get_flags(enc)
                                        & EVP_CIPH_FLAG_AEAD_CIPHER) != 0,
                                        rl->libctx))
                    return 0;
            }
        }
    }
    return 1;
}

static int tls1_mac(OSSL_RECORD_LAYER *rl, SSL3_RECORD *rec, unsigned char *md,
                    int sending)
{
    unsigned char *seq = rl->sequence;
    EVP_MD_CTX *hash;
    size_t md_size;
    int i;
    EVP_MD_CTX *hmac = NULL, *mac_ctx;
    unsigned char header[13];
    int t;
    int ret = 0;

    hash = rl->md_ctx;

    t = EVP_MD_CTX_get_size(hash);
    if (!ossl_assert(t >= 0))
        return 0;
    md_size = t;

    if (rl->stream_mac) {
        mac_ctx = hash;
    } else {
        hmac = EVP_MD_CTX_new();
        if (hmac == NULL || !EVP_MD_CTX_copy(hmac, hash)) {
            goto end;
        }
        mac_ctx = hmac;
    }

    if (!rl->isdtls
            && rl->tlstree
            && EVP_MD_CTX_ctrl(mac_ctx, EVP_MD_CTRL_TLSTREE, 0, seq) <= 0)
        goto end;

    if (rl->isdtls) {
        unsigned char dtlsseq[8], *p = dtlsseq;

        s2n(rl->epoch, p);
        memcpy(p, &seq[2], 6);

        memcpy(header, dtlsseq, 8);
    } else {
        memcpy(header, seq, 8);
    }

    header[8] = rec->type;
    header[9] = (unsigned char)(rl->version >> 8);
    header[10] = (unsigned char)(rl->version);
    header[11] = (unsigned char)(rec->length >> 8);
    header[12] = (unsigned char)(rec->length & 0xff);

    if (!sending && !rl->use_etm
        && EVP_CIPHER_CTX_get_mode(rl->enc_ctx) == EVP_CIPH_CBC_MODE
        && ssl3_cbc_record_digest_supported(mac_ctx)) {
        OSSL_PARAM tls_hmac_params[2], *p = tls_hmac_params;

        *p++ = OSSL_PARAM_construct_size_t(OSSL_MAC_PARAM_TLS_DATA_SIZE,
                                           &rec->orig_len);
        *p++ = OSSL_PARAM_construct_end();

        if (!EVP_PKEY_CTX_set_params(EVP_MD_CTX_get_pkey_ctx(mac_ctx),
                                     tls_hmac_params))
            goto end;
    }

    if (EVP_DigestSignUpdate(mac_ctx, header, sizeof(header)) <= 0
        || EVP_DigestSignUpdate(mac_ctx, rec->input, rec->length) <= 0
        || EVP_DigestSignFinal(mac_ctx, md, &md_size) <= 0)
        goto end;

    OSSL_TRACE_BEGIN(TLS) {
        BIO_printf(trc_out, "seq:\n");
        BIO_dump_indent(trc_out, seq, 8, 4);
        BIO_printf(trc_out, "rec:\n");
        BIO_dump_indent(trc_out, rec->data, rec->length, 4);
    } OSSL_TRACE_END(TLS);

    if (!rl->isdtls) {
        for (i = 7; i >= 0; i--) {
            ++seq[i];
            if (seq[i] != 0)
                break;
        }
    }
    OSSL_TRACE_BEGIN(TLS) {
        BIO_printf(trc_out, "md:\n");
        BIO_dump_indent(trc_out, md, md_size, 4);
    } OSSL_TRACE_END(TLS);
    ret = 1;
 end:
    EVP_MD_CTX_free(hmac);
    return ret;
}

/* TLSv1.0, TLSv1.1 and TLSv1.2 all use the same funcs */
struct record_functions_st tls_1_funcs = {
    tls1_set_crypto_state,
    tls_default_read_n,
    tls_get_more_records,
    tls1_cipher,
    tls1_mac,
    tls_default_set_protocol_version,
    tls_default_validate_record_header,
    tls_default_post_process_record
};

struct record_functions_st dtls_1_funcs = {
    tls1_set_crypto_state,
    tls_default_read_n,
    dtls_get_more_records,
    tls1_cipher,
    tls1_mac,
    tls_default_set_protocol_version,
    NULL,
    NULL
};
