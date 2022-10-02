#if !defined(INC_H___39772E69_D8E9_41B9_A0EF_1CFDEE64ED11__HEAD__)
#define INC_H___39772E69_D8E9_41B9_A0EF_1CFDEE64ED11__HEAD__

namespace shared {

	class Openssl final {
	public:
		static bool MD5(const std::string input, std::string& output, const std::vector<std::uint8_t> seed = {}, const bool& hex_output = true, const bool& base64_output = false);
		static bool HMAC_SHA1(const std::string& input, std::string& output, const std::string& seed, const bool& hex_output = true, const bool& base64_output = false);
		static bool HMAC_SHA256(const std::string& input, std::string& output, const std::string& seed, const bool& hex_output = true, const bool& base64_output = false);
		static bool SHA256(const std::string& inData, const bool& isHex, std::string& outData);
		static bool Base64Encode(const std::string& input, std::string& output, const bool& multiline = false);
		static bool Base64Decode(const std::string& input, std::string& output, const bool& multiline = false);
		//!@ Made a Chrome Extensions ID
		static bool ChromeExtensionsIDGenerate(std::string& outRSAPrivate_pkcs8, std::string& outRSAPublic_pkcs8, std::string& outRSAPublicDER, std::string& outExtensionsID);
		static bool ChromeExtensionsIDGet(const std::string& inRSAPrivate_pkcs8, std::string& outRSAPublic_pkcs8, std::string& outRSAPublicDER, std::string& outExtensionsID);
	public:
		class Rsa final {
		public:
			enum struct EnType : std::uint64_t {
				EN_TYPE_PKCS1 = 1,
				EN_TYPE_PKCS8 = 8,
			};
		public:
			static bool Generate(const EnType& type, const std::string& pwd, std::string& outKeyPublic, std::string& outKeyPrivate);
			static bool PrivateToPublic(const EnType& type, const std::string& pwd, const std::string& inKeyPrivate, std::string& outKeyPublic);
			static bool DERPublic(const EnType& type, const std::string& keyPublic, const std::string& pwd, std::string& outDER);
			static bool Encode(const EnType& type, const std::string& pwd, const std::string& keyPublic, const std::string& keyPrivate, const std::string& inData, std::string& outData);
			static bool Decode(const EnType& type, const std::string& pwd, const std::string& keyPublic, const std::string& keyPrivate, const std::string& inData, std::string& outData);
		};
	};
}///namespace shared

#pragma comment(lib,"openssl")

/// /*新生®（上海）**/
/// /*2022_03_29T12:38:57.0964948Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___39772E69_D8E9_41B9_A0EF_1CFDEE64ED11__HEAD__
