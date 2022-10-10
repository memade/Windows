#if !defined(INC_H___C3645F03_5567_4DB9_A548_890859855E2E__HEAD__)
#define INC_H___C3645F03_5567_4DB9_A548_890859855E2E__HEAD__

namespace libiconv {
#include <source/include/iconv.h>

	class IConv {
	public:
		static std::wstring gbk_to_ucs2le(const std::string&);
		static std::string ucs2le_to_gbk(const std::wstring&);
	};

}///namespace libiconv


#pragma comment(lib,"iconv.lib")
#pragma comment(lib,"libiconv_static.lib")
/// /*新生®（上海）**/
/// /*2022_10_10T01:00:04.2801093Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___C3645F03_5567_4DB9_A548_890859855E2E__HEAD__
