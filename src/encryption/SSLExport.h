#if !defined(AFX_SK_H__13BFDE77_FA81_402B_A58A_9E56A152A63F__HEAD__)
#define AFX_SK_H__13BFDE77_FA81_402B_A58A_9E56A152A63F__HEAD__

#include <openssl/err.h>
//#include <openssl/types.h>
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

namespace shared {
	namespace encrypt {

		enum struct EnRSAType : std::uint64_t {
			EN_RSATYPE_PKCS1 = 1,
			EN_RSATYPE_PKCS8 = 8,
		};

		class OpenSSL final {
			std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
		public:
			OpenSSL();
			~OpenSSL();
		public:
			bool Dev();
			bool RSAGenerate(
				__in const std::uint64_t& RSAType,
				__in const std::string& pwd,
				__out std::string& keyPublic,
				__out std::string& keyPrivate) const;
			bool RSAGeneratePublic(
				__in const std::uint64_t& RSAType,
				__in const std::string& pwd,
				__in const std::string& keyPrivate,
				__out std::string& keyPublic) const;
			bool RSADecode(
				__in const std::uint64_t& RSAType,
				__in const std::string& pwd,
				__in const std::string& keyPublic,
				__in const std::string& keyPrivate,
				__in const std::string& inData,
				__out std::string& outData) const;
			bool RSAEncode(
				__in const std::uint64_t& RSAType,
				__in const std::string& pwd,
				__in const std::string& keyPublic,
				__in const std::string& keyPrivate,
				__in const std::string& inData,
				__out std::string& outData) const;
			bool Sha256(
				__in const std::string& inData,
				__in const bool& isHex,
				__out std::string& outData) const;
			bool RSADERPublic(
				__in const std::uint64_t& RSAType,
				__in const std::string& keyPublic,
				__in const std::string& pwd,
				__out std::string& outDER) const;
			bool Base64Encode(
				__in const std::string& inData,
				__in const bool& newLine,
				__out std::string& outData) const;
			bool Base64Decode(
				__in const std::string& inData,
				__in const bool& newLine,
				__out std::string&) const;
			bool MD5(
				__in const std::string& inData,
				__in const std::string& seed,
				__out std::string& outData) const;
			bool HMAC(
				__in const std::string& message,
				__out std::string& hex_encode_string,
				__in const std::uint8_t* seed = nullptr,
				__in const size_t& nseed = 0) const;
		};
	}///namespace encrypt
}///namespace shared

/*!@ 新生联创®（上海）*/
/*!@ Tue Nov 2 16:21:45 UTC+0800 2021*/
/*!@ ___www.skstu.com___*/
#endif /*AFX_SK_H__13BFDE77_FA81_402B_A58A_9E56A152A63F__HEAD__*/