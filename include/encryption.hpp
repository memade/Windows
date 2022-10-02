#if !defined(AFX_SK_H__C264B416_7324_4CB3_8816_B96515F855A3__HEAD__)
#define AFX_SK_H__C264B416_7324_4CB3_8816_B96515F855A3__HEAD__

namespace shared {

	class Encryption final {
	public:
		class base64 final {
		public:
			base64();
			~base64();
		public:
			static std::string base64_encode(const std::string& src);
			static std::string base64_decode(const std::string& encoded_string);
			static bool base64_decode(const std::string& encoded_string, std::string& decoded_string);
			static bool base64_decode(const unsigned char* in_data, const size_t& in_data_size, unsigned char* out_buffer, const size_t& out_buffer_size);
		};
	public:
		static std::string WemadeEncode(const std::string& strSrc);
		static std::string WemadeDecode(const std::string& in);
	};
}///namespace shared

#pragma comment(lib,"encryption")
/*!@ 新生联创®（上海）*/
/*!@ Fri Jul 30 11:07:30 UTC+0800 2021*/
/*!@ ___www.skstu.com___*/
#endif /*AFX_SK_H__C264B416_7324_4CB3_8816_B96515F855A3__HEAD__*/