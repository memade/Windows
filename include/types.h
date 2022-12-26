#if !defined(INC_H___CD8CAF0D_B997_4824_A19E_3198CAC9E5D0__HEAD__)
#define INC_H___CD8CAF0D_B997_4824_A19E_3198CAC9E5D0__HEAD__

#ifndef _UNICODE
#define xstring  std::string
#define tostring std::to_string
#else
#define xstring  std::wstring
#define tostring std::to_wstring
#endif

template<typename T>
class stl_container_t final {
public:
 typedef stl_container_t<T> self;
 typedef T source_type;
 stl_container_t() :m_source() {}
 explicit stl_container_t(const T& source) : m_source(source) {}
 ~stl_container_t() { m_source.clear(); }
public:
 const T& operator*() const noexcept { return get(); }
 const T& operator()() const noexcept { return get(); }
 const T* operator->() const noexcept { return &get(); }
private:
 const T& get() const noexcept { return m_source; }
private:
 T m_source;
};

#if _UNICODE
using TString = stl_container_t<std::wstring>;
#else
using TString = stl_container_t<std::string>;
#endif
using TStringA = stl_container_t<std::string>;
using TSringW = stl_container_t<std::wstring>;

using tfRouteIndex = unsigned long;
using tfRoutePak = std::string;
using tfRouteRes = std::map<tfRouteIndex, tfRoutePak>;
using tfCommandLineNode = std::map<std::string, std::string>;
using tfEnumFolderNode = std::map<std::string, std::string>;
using tfEnumFolderCb = std::function<void(const std::string& pathname,const std::string& identify,const bool& is_directory)>;
static
const
unsigned char
__XSLC_TEAM_TENET_[] = \
{ 0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x20,0xB0,0xB2,0xC8,0xAB,0xA1,0xA2,0xCE,0xC8,0xB6,0xA8,0xA1,0xA2,0xC9,0xF9,0xD3,0xFE,0x20,0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x00 };
static
const
unsigned char
__XSLC_TEAM_SIGN_[] = \
{ 0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x20,0xD0,0xC2,0xC9,0xFA,/*0xC1,0xAA,0xB4,0xB4,*/0xA3,0xA8,0xC9,0xCF,0xBA,0xA3,0xA3,0xA9,0x20,0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x2D,0x00 };
/// /*新生®（上海）**/
/// /*2022_02_19T06:20:27.8212221Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___CD8CAF0D_B997_4824_A19E_3198CAC9E5D0__HEAD__