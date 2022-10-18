#if !defined(INC_H___586DABC5_153C_4C66_96BB_F260D1ADC3C8__HEAD__)
#define INC_H___586DABC5_153C_4C66_96BB_F260D1ADC3C8__HEAD__

#define SPDLOG_ENABLE_EXTEND 1
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/async.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/fmt/ostr.h>

namespace shared {

 enum class EnLogType : unsigned long long {
  EN_LOG_TYPE_UNDEFINED = 0,
  EN_LOG_TYPE_BEGIN = 1,
  EN_LOG_TYPE_TRACE = EN_LOG_TYPE_BEGIN << 1,
  EN_LOG_TYPE_ERROR = EN_LOG_TYPE_BEGIN << 2,
  EN_LOG_TYPE_WARN = EN_LOG_TYPE_BEGIN << 3,
  EN_LOG_TYPE_FAILED = EN_LOG_TYPE_BEGIN << 4,
  EN_LOG_TYPE_SUCCESS = EN_LOG_TYPE_BEGIN << 5,
  EN_LOG_TYPE_SIGNAL = EN_LOG_TYPE_BEGIN << 6,
  EN_LOG_TYPE_FOLLOW = EN_LOG_TYPE_BEGIN << 7,
  EN_LOG_TYPE_SYSTEM = EN_LOG_TYPE_BEGIN << 8,
  EN_LOG_TYPE_NORMAL = EN_LOG_TYPE_BEGIN << 9,

  EN_LOG_TYPE_ALL = EN_LOG_TYPE_TRACE | EN_LOG_TYPE_ERROR | EN_LOG_TYPE_WARN | \
  EN_LOG_TYPE_FAILED | EN_LOG_TYPE_SUCCESS | EN_LOG_TYPE_SIGNAL | \
  EN_LOG_TYPE_FOLLOW | EN_LOG_TYPE_SYSTEM,

  EN_LOG_TYPE_SIMPLE = EN_LOG_TYPE_TRACE | EN_LOG_TYPE_ERROR | EN_LOG_TYPE_WARN | \
  EN_LOG_TYPE_FAILED | EN_LOG_TYPE_SUCCESS | EN_LOG_TYPE_SYSTEM,
 };

 bool operator&(const EnLogType&, const EnLogType&);
 EnLogType operator|(const EnLogType&, const unsigned long long&);
 EnLogType operator|(const EnLogType&, const EnLogType&);
 EnLogType operator|(const unsigned long long&, const EnLogType&);
 void operator|=(EnLogType&, const EnLogType&);
 EnLogType ParseLogTypes(const EnLogType&);

 class ISpdlog {
 public:
  static ISpdlog* CreateInterface(
   const std::string& module_name,
   const std::string& logs_path,
   const EnLogType& logTypes = EnLogType::EN_LOG_TYPE_SYSTEM, 
   const std::string& logFileFormat = R"(.log)",
   const spdlog::tf_spdlog_output_cb& spdlog_output_cb = nullptr);

  static void DestoryInterface(ISpdlog*&);
  static void DestoryInterface();
 public:
  ISpdlog() {}
  ~ISpdlog() {}
  virtual void Release() const = 0;
 public:
  template<typename... Args>
  inline static std::wstring FORMAT(const spdlog::wstring_view_t& fmt, const Args&... args);
  template<typename... Args>
  inline static std::string FORMAT(const spdlog::string_view_t& fmt, const Args&... args);
 public:
  virtual void LOG(const std::string&, const EnLogType&) = 0;
  virtual void LOG(const std::wstring&, const EnLogType&) = 0;
 };

 template<typename... Args>
 inline static std::wstring ISpdlog::FORMAT(const spdlog::wstring_view_t& fmt, const Args&... args) {
  std::wstring result;
  try {
   fmt::wmemory_buffer wbuf;
   fmt::format_to(wbuf, fmt, args...);
   result.append(wbuf.data(), wbuf.size());
  }
  catch (const spdlog::spdlog_ex& ex) {
   std::cout << ex.what() << std::endl;
  }
  return result;
 }
 template<typename... Args>
 inline static std::string ISpdlog::FORMAT(const spdlog::string_view_t& fmt, const Args&... args) {
  std::string result;
  try {
   fmt::memory_buffer buf;
   fmt::format_to(buf, fmt, args...);
   result.append(buf.data(), buf.size());
  }
  catch (const spdlog::spdlog_ex& ex) {
   std::cout << ex.what() << std::endl;
  }
  return result;
 }

}///namespace shared

extern shared::ISpdlog* __gpSpdlog;

#define FMT shared::ISpdlog::FORMAT

template<typename... Args>
inline static void LOGERROR(const spdlog::string_view_t& fmt, const Args&... args) {
 do {
  if (!__gpSpdlog)
   break;
  std::string result;
  try {
   fmt::memory_buffer wbuf;
   fmt::format_to(wbuf, fmt, args...);
   result.append(wbuf.data(), wbuf.size());
  }
  catch (const spdlog::spdlog_ex& ex) {
   std::cout << ex.what() << std::endl;
  }
  if (result.empty())
   break;
  __gpSpdlog->LOG(result, shared::EnLogType::EN_LOG_TYPE_ERROR);
 } while (0);
}

template<typename... Args>
inline static void LOGERROR(const spdlog::wstring_view_t& fmt, const Args&... args) {
 do {
  if (!__gpSpdlog)
   break;
  std::wstring result;
  try {
   fmt::wmemory_buffer wbuf;
   fmt::format_to(wbuf, fmt, args...);
   result.append(wbuf.data(), wbuf.size());
  }
  catch (const spdlog::spdlog_ex& ex) {
   std::cout << ex.what() << std::endl;
  }
  if (result.empty())
   break;
  __gpSpdlog->LOG(result, shared::EnLogType::EN_LOG_TYPE_ERROR);
 } while (0);
}

template<typename... Args>
inline static void LOGINFO(const spdlog::string_view_t& fmt, const Args&... args) {
 do {
  if (!__gpSpdlog)
   break;
  std::string result;
  try {
   fmt::memory_buffer wbuf;
   fmt::format_to(wbuf, fmt, args...);
   result.append(wbuf.data(), wbuf.size());
  }
  catch (const spdlog::spdlog_ex& ex) {
   std::cout << ex.what() << std::endl;
  }
  if (result.empty())
   break;
  __gpSpdlog->LOG(result, shared::EnLogType::EN_LOG_TYPE_SYSTEM);
 } while (0);
}

template<typename... Args>
inline static void LOGINFO(const spdlog::wstring_view_t& fmt, const Args&... args) {
 do {
  if (!__gpSpdlog)
   break;
  std::wstring result;
  try {
   fmt::wmemory_buffer wbuf;
   fmt::format_to(wbuf, fmt, args...);
   result.append(wbuf.data(), wbuf.size());
  }
  catch (const spdlog::spdlog_ex& ex) {
   std::cout << ex.what() << std::endl;
  }
  if (result.empty())
   break;
  __gpSpdlog->LOG(result, shared::EnLogType::EN_LOG_TYPE_SYSTEM);
 } while (0);
}

template<typename... Args>
inline static void LOGWARN(const spdlog::string_view_t& fmt, const Args&... args) {
 do {
  if (!__gpSpdlog)
   break;
  std::string result;
  try {
   fmt::memory_buffer wbuf;
   fmt::format_to(wbuf, fmt, args...);
   result.append(wbuf.data(), wbuf.size());
  }
  catch (const spdlog::spdlog_ex& ex) {
   std::cout << ex.what() << std::endl;
  }
  if (result.empty())
   break;
  __gpSpdlog->LOG(result, shared::EnLogType::EN_LOG_TYPE_WARN);
 } while (0);
}

template<typename... Args>
inline static void LOGWARN(const spdlog::wstring_view_t& fmt, const Args&... args) {
 do {
  if (!__gpSpdlog)
   break;
  std::wstring result;
  try {
   fmt::wmemory_buffer wbuf;
   fmt::format_to(wbuf, fmt, args...);
   result.append(wbuf.data(), wbuf.size());
  }
  catch (const spdlog::spdlog_ex& ex) {
   std::cout << ex.what() << std::endl;
  }
  if (result.empty())
   break;
  __gpSpdlog->LOG(result, shared::EnLogType::EN_LOG_TYPE_WARN);
 } while (0);
}



#pragma comment(lib,"spdlog.lib")
/// /*新生®（上海）**/
/// /*2022_09_26T22:41:11.7953709Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___586DABC5_153C_4C66_96BB_F260D1ADC3C8__HEAD__




