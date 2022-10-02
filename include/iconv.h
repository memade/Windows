#if !defined(INC_H___5ABFD307_8EBE_4F10_8105_13616F0B7084__HEAD__)
#define INC_H___5ABFD307_8EBE_4F10_8105_13616F0B7084__HEAD__

#if 0
namespace libconv {
#include <libiconv/source/include/iconv.h>
#pragma comment(lib,"libiconv.lib")
}///namespace libconv
#endif

namespace shared {
	class IConv final {
	public:
		IConv() {}
		~IConv() = delete;
	public:
		static std::string ToLowerA(const std::string& src) {
			std::string result;
			do {
				if (src.empty())
					break;
				result = src;
				std::transform(result.begin(), result.end(), result.begin(), ::tolower);
			} while (0);
			return result;
		}
		static std::wstring ToLowerW(const std::wstring& src) {
			std::wstring result;
			do {
				if (src.empty())
					break;
				result = src;
				std::transform(result.begin(), result.end(), result.begin(), ::tolower);
			} while (0);
			return result;
		}
		static std::string ToUpperA(const std::string& src) {
			std::string result;
			do {
				if (src.empty())
					break;
				result = src;
				std::transform(result.begin(), result.end(), result.begin(), ::toupper);
			} while (0);
			return result;
		}

		static std::wstring ToUpperW(const std::wstring& src) {
			std::wstring result;
			do {
				if (src.empty())
					break;
				result = src;
				std::transform(result.begin(), result.end(), result.begin(), ::toupper);
			} while (0);
			return result;
		}

		static std::string WStringToUTF8(const std::wstring& src) {
			std::string result;
			char* pElementText = nullptr;
			do {
				if (src.empty())
					break;
				size_t iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)src.data(), -1, NULL, 0, NULL, NULL);
				pElementText = new char[iTextLen + 1];
				memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
				::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)src.data(), -1, pElementText, (int)iTextLen, NULL, NULL);
				result = pElementText;
			} while (0);
			if (pElementText) {
				delete[] pElementText;
				pElementText = nullptr;
			}
			return result;
		}

		static std::wstring UTF8ToWString(const std::string& src) {
			std::wstring result;
			wchar_t* pUnicode = nullptr;
			do {
				if (src.empty())
					break;
				size_t unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, src.data(), -1, NULL, 0);
				pUnicode = new wchar_t[unicodeLen + 1];
				::memset((void*)pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
				::MultiByteToWideChar(CP_UTF8, 0, src.data(), -1, (LPWSTR)pUnicode, (int)unicodeLen);
				result = pUnicode;
			} while (0);
			if (pUnicode) {
				delete[] pUnicode;
				pUnicode = nullptr;
			}
			return result;
		}

		static std::string MBytesToUTF8(const std::string& src) {
			std::string result;
			do {
				if (src.empty())
					break;
				auto unicode = MBytesToWString(src);
				if (unicode.empty())
					break;
				result = WStringToUTF8(unicode);
			} while (0);
			return result;
		}

		static std::string UTF8ToMBytes(const std::string& src) {
			std::string result;
			do {
				if (src.empty())
					break;
				auto unicode = UTF8ToWString(src);
				if (unicode.empty())
					break;
				result = WStringToMBytes(unicode);
			} while (0);
			return result;
		}

		static std::string WStringToMBytes(const std::wstring& src, const std::locale& locale = std::locale{ "" }) {
			std::string result;
			char* pElementText = nullptr;
			do {
				if (src.empty())
					break;
				size_t iTextLen = ::WideCharToMultiByte(CP_ACP, 0, src.data(), -1, NULL, 0, NULL, NULL);
				pElementText = new char[iTextLen + 1];
				memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
				::WideCharToMultiByte(CP_ACP, 0, src.data(), -1, pElementText, (int)iTextLen, NULL, NULL);
				result = pElementText;
			} while (0);
			if (pElementText) {
				delete[] pElementText;
				pElementText = nullptr;
			}
			return result;
		}

		static std::wstring MBytesToWString(const std::string& src, const std::locale& locale = std::locale{ "" }) {
			std::wstring result;
			wchar_t* pUnicode = nullptr;
			do {
				if (src.empty())
					break;
				size_t unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, src.data(), -1, NULL, 0);
				pUnicode = new wchar_t[unicodeLen + 1];
				memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
				::MultiByteToWideChar(CP_ACP, 0, src.data(), -1, (LPWSTR)pUnicode, (int)unicodeLen);
				result = (wchar_t*)pUnicode;
			} while (0);
			if (pUnicode) {
				delete[] pUnicode;
				pUnicode = nullptr;
			}
			return result;
		}

#if 0
		static std::string unicode_to_gbk(const std::wstring&) {
			return "";
		}
		static std::wstring gbk_to_unicode(const std::string& gbk) {
			std::wstring result;
			libconv::libiconv_t iconobj = nullptr;
			do {
				if (gbk.empty())
					break;
				iconobj = libconv::iconv_open("ucs-2le", "gbk");
				if (!iconobj)
					break;
				size_t dummy = 0;
				libconv::iconv(iconobj, NULL, NULL, NULL, &dummy);
				size_t nGbk = gbk.size();
				size_t nUnicode = nGbk * sizeof(wchar_t);
				result.resize(nUnicode, 0x00);
				char* pGbk = (char*)&gbk[0];
				char* pUnicode = (char*)&result[0];
				size_t nRet = libconv::iconv(iconobj, &pGbk, &nGbk, &pUnicode, &nUnicode);
				if (nRet <= 0) {
					result.clear();
					break;
				}
				auto sks = 0;
				//STRING_RESIZE(result);
			} while (0);
			if (iconobj) {
				libconv::iconv_close(iconobj);
				iconobj = nullptr;
			}
			return result;
		}
#endif





	};
}


/// /*新生®（上海）**/
/// /*2022_02_19T09:06:04.6804495Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___5ABFD307_8EBE_4F10_8105_13616F0B7084__HEAD__