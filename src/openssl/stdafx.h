#if !defined(INC_H___1BB021C1_D00A_428A_A9B2_3336C7596320__HEAD__)
#define INC_H___1BB021C1_D00A_428A_A9B2_3336C7596320__HEAD__

#define ENABLE_OPENSSL_3 1
#if ENABLE_OPENSSL_3
#pragma warning(disable: 4996)
#endif

#include <win.h>
#include <openssl.hpp>

#include <openssl/err.h>
#if ENABLE_OPENSSL_3
#include <openssl/types.h>
#endif
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/hmac.h>
#include <openssl/md2.h>
#include <openssl/md5.h>
#include <openssl/rc5.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/buffer.h>

#if ENABLE_OPENSSL_3
#pragma comment(lib,"wldap32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Crypt32.lib")
#pragma comment(lib,"Normaliz.lib")
#pragma comment(lib,"libssl.lib")
#pragma comment(lib,"libcrypto.lib")
#else
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")
#endif

/// /*新生®（上海）**/
/// /*2022_03_29T12:34:53.4669092Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___1BB021C1_D00A_428A_A9B2_3336C7596320__HEAD__

