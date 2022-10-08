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

namespace spdlog {
#define SPDLOG_FMT_T spdlog::Log::Format

#pragma pack(push,1)
 enum class EnLogType : unsigned long long {
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
#pragma pack(pop)

 inline EnLogType operator|(const EnLogType& obj1, const unsigned long long& obj2)
 {
  return EnLogType(unsigned long long(obj1) | obj2);
 }
 inline EnLogType operator|(const EnLogType& obj1, const EnLogType& obj2)
 {
  return EnLogType(unsigned long long(obj1) | unsigned long long(obj2));
 }
 inline EnLogType operator|(const unsigned long long& obj1, const EnLogType& obj2)
 {
  return EnLogType(obj1 | unsigned long long(obj2));
 }
 inline bool operator&(const EnLogType& obj1, const EnLogType& obj2)
 {
  return unsigned long long(obj1) & unsigned long long(obj2);
 }

 class Log final
 {
 public:
  Log(const std::string& path, spdlog::tf_spdlog_output_cb outputcb = nullptr) : m_LogPathBase(path + "/logs/"), m_outputcb(outputcb) {}
  ~Log() { Free(); }

  std::shared_ptr<spdlog::logger>
   log(const std::string& process, EnLogType logType = EnLogType::EN_LOG_TYPE_SYSTEM)
  {
   return m_pSpdlogPtrQ[process][logType];
  }

  //!签名 : log
  //!说明 : 
  //!注意 : 
  //!日期 : Mon Nov 16 10:07:22 UTC+0800 2020
  void log(const std::string& key, const EnLogType& type, const unsigned int& level, const std::string& logdata)
  {
   if (key.empty() || logdata.empty())
    return;
   m_pSpdlogPtrQ.search(
    key,
    [&](const auto&, auto& loggers)
    {
     loggers.search(
      type,
      [&](const EnLogType&, std::shared_ptr<spdlog::logger>& logger)
      {
       switch (spdlog::level::level_enum(level))
       {
       case spdlog::level::level_enum::info:
       {
        logger->info(logdata);
       }break;
       case spdlog::level::level_enum::trace:
       {
        logger->trace(logdata);
       }break;
       case spdlog::level::level_enum::debug:
       {
        logger->debug(logdata);
       }break;
       case spdlog::level::level_enum::warn:
       {
        logger->warn(logdata);
       }break;
       case spdlog::level::level_enum::err:
       {
        logger->error(logdata);
       }break;
       case spdlog::level::level_enum::critical:
       {
        logger->critical(logdata);
       }break;
       default:
       {
       }break;
       }///switch
      });
    });
  }
 public:
  void MsgCb(const spdlog::tf_spdlog_output_cb& outputcb)
  {
   m_outputcb = outputcb;
   m_pSpdlogPtrQ.iterate(
    [&](const auto& process, auto& loggers)
    {
     loggers.iterate(
      [&](const auto&, auto& logger)
      {
       logger->set_output_cb(m_outputcb);
      });
    });
  }


  //!签名 : Alloc
  //!说明 : 初始化进程日志
  //!注意 : 返回进程标识符(一般为进程名字)
  //!日期 : Mon Nov 16 09:52:56 UTC+0800 2020
  const std::string Alloc(const std::string& process_name, const EnLogType& LogTypes)
  {
   auto process = process_name.substr(0, process_name.rfind("."));
   //!@ 移除空格
   for (auto it = process.begin(); it != process.end();) {
    if (*it == ' ')
     it = process.erase(it);
    else
     ++it;
   }
   std::string module_name;
   shared::container::map<EnLogType, std::shared_ptr<spdlog::logger>> processLoggers;

   {
    module_name = "system";
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>((m_LogPathBase + process + "//" + module_name + ".log").c_str(), 0, 0));
    auto result = std::make_shared<spdlog::logger>(module_name, begin(sinks), end(sinks));
#ifdef _DEBUG //! 设置日志记录级别
    result->set_level(spdlog::level::trace);
#else
    result->set_level(spdlog::level::info);
#endif
    result->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");
    /*spLogger->set_pattern("[%Y%m%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");*/
    result->flush_on(spdlog::level::trace);
    processLoggers.push(EnLogType::EN_LOG_TYPE_SYSTEM, result);
   }


   {
    module_name = "normal";
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>((m_LogPathBase + process + "//" + module_name + ".log").c_str(), 0, 0));
    auto result = std::make_shared<spdlog::logger>(module_name, begin(sinks), end(sinks));
#ifdef _DEBUG //! 设置日志记录级别
    result->set_level(spdlog::level::trace);
#else
    result->set_level(spdlog::level::info);
#endif
    auto new_formatter = details::make_unique<pattern_formatter>("%^%v%$", pattern_time_type::local, "");
    result->set_formatter(std::move(new_formatter));

    //result->set_pattern("%^%v%$");
    result->flush_on(spdlog::level::trace);
    processLoggers.push(EnLogType::EN_LOG_TYPE_NORMAL, result);
   }

   if (LogTypes & EnLogType::EN_LOG_TYPE_ERROR)
   {
    module_name = "error";
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>((m_LogPathBase + process + "//" + module_name + ".log").c_str(), 0, 0));
    auto result = std::make_shared<spdlog::logger>(module_name, begin(sinks), end(sinks));
#ifdef _DEBUG //! 设置日志记录级别
    result->set_level(spdlog::level::trace);
#else
    result->set_level(spdlog::level::info);
#endif
    result->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");
    result->flush_on(spdlog::level::trace);
    processLoggers.push(EnLogType::EN_LOG_TYPE_ERROR, result);
   }
   if (LogTypes & EnLogType::EN_LOG_TYPE_WARN)
   {
    module_name = "warn";
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>((m_LogPathBase + process + "//" + module_name + ".log").c_str(), 0, 0));
    auto result = std::make_shared<spdlog::logger>(module_name, begin(sinks), end(sinks));
#ifdef _DEBUG //! 设置日志记录级别
    result->set_level(spdlog::level::trace);
#else
    result->set_level(spdlog::level::info);
#endif
    result->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");
    result->flush_on(spdlog::level::trace);
    processLoggers.push(EnLogType::EN_LOG_TYPE_WARN, result);
   }
   if (LogTypes & EnLogType::EN_LOG_TYPE_TRACE)
   {
    module_name = "trace";
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>((m_LogPathBase + process + "//" + module_name + ".log").c_str(), 0, 0));
    auto result = std::make_shared<spdlog::logger>(module_name, begin(sinks), end(sinks));
#ifdef _DEBUG //! 设置日志记录级别
    result->set_level(spdlog::level::trace);
#else
    result->set_level(spdlog::level::info);
#endif
    result->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");
    result->flush_on(spdlog::level::trace);
    processLoggers.push(EnLogType::EN_LOG_TYPE_TRACE, result);
   }
   if (LogTypes & EnLogType::EN_LOG_TYPE_SUCCESS)
   {
    module_name = "success";
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>((m_LogPathBase + process + "//" + module_name + ".log").c_str(), 0, 0));
    auto result = std::make_shared<spdlog::logger>(module_name, begin(sinks), end(sinks));
#ifdef _DEBUG //! 设置日志记录级别
    result->set_level(spdlog::level::trace);
#else
    result->set_level(spdlog::level::info);
#endif
    result->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");
    result->flush_on(spdlog::level::trace);
    processLoggers.push(EnLogType::EN_LOG_TYPE_SUCCESS, result);
   }
   if (LogTypes & EnLogType::EN_LOG_TYPE_FAILED)
   {
    module_name = "failed";
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>((m_LogPathBase + process + "//" + module_name + ".log").c_str(), 0, 0));
    auto result = std::make_shared<spdlog::logger>(module_name, begin(sinks), end(sinks));
#ifdef _DEBUG //! 设置日志记录级别
    result->set_level(spdlog::level::trace);
#else
    result->set_level(spdlog::level::info);
#endif
    result->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");
    result->flush_on(spdlog::level::trace);
    processLoggers.push(EnLogType::EN_LOG_TYPE_FAILED, result);
   }
   if (LogTypes & EnLogType::EN_LOG_TYPE_SIGNAL)
   {
    module_name = "signal";
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>((m_LogPathBase + process + "//" + module_name + ".log").c_str(), 0, 0));
    auto result = std::make_shared<spdlog::logger>(module_name, begin(sinks), end(sinks));
#ifdef _DEBUG //! 设置日志记录级别
    result->set_level(spdlog::level::trace);
#else
    result->set_level(spdlog::level::info);
#endif
    result->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");
    result->flush_on(spdlog::level::trace);
    processLoggers.push(EnLogType::EN_LOG_TYPE_SIGNAL, result);
   }
   if (LogTypes & EnLogType::EN_LOG_TYPE_FOLLOW)
   {
    module_name = "follow";
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>((m_LogPathBase + process + "//" + module_name + ".log").c_str(), 0, 0));
    auto result = std::make_shared<spdlog::logger>(module_name, begin(sinks), end(sinks));
#ifdef _DEBUG //! 设置日志记录级别
    result->set_level(spdlog::level::trace);
#else
    result->set_level(spdlog::level::info);
#endif
    result->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");
    result->flush_on(spdlog::level::trace);
    processLoggers.push(EnLogType::EN_LOG_TYPE_FOLLOW, result);
   }
   if (processLoggers.empty())
    return "";
   m_pSpdlogPtrQ.push(process, processLoggers);
   log(process)->set_output_cb(m_outputcb);
   log(process)->info("{}", __XSLC_TEAM_TENET_);
   return process;
  }
  void FreeAt(const std::string& process) {
   m_pSpdlogPtrQ.pop(
    process,
    [&](const auto&, auto& modules) {
     modules[EnLogType::EN_LOG_TYPE_SYSTEM]->info(__XSLC_TEAM_TENET_);
    });
  }
  void Free()
  {
   m_pSpdlogPtrQ.iterate_clear(
    [&](const auto& process, auto& modules, auto& itbreak, auto& itclear)
    {
     modules[EnLogType::EN_LOG_TYPE_SYSTEM]->info(__XSLC_TEAM_TENET_);
     itclear = true;
    });
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
   spdlog::flush_every(std::chrono::seconds(30));
   spdlog::shutdown();
   spdlog::drop_all();
  }
 public:
  const std::string LogPath() const { return m_LogPathBase; }
  template<typename... Args>
  inline static std::wstring Format(spdlog::wstring_view_t fmt, const Args&... args);
  template<typename... Args>
  inline static std::string Format(spdlog::string_view_t fmt, const Args&... args);
 private:
  std::string m_LogPathBase;
  spdlog::tf_spdlog_output_cb m_outputcb = nullptr;
  shared::container::map<std::string, shared::container::map<EnLogType, std::shared_ptr<spdlog::logger>>> m_pSpdlogPtrQ;
 };

 template<typename... Args>
 inline static std::wstring Log::Format(spdlog::wstring_view_t fmt, const Args&... args) {
  try {
   fmt::wmemory_buffer wbuf;
   fmt::format_to(wbuf, fmt, args...);
   return std::wstring(wbuf.data(), wbuf.size());
  }
  catch (const spdlog::spdlog_ex& ex) {
   std::cout << "Log initialization failed: " << ex.what() << std::endl;
   return L"";
  }
 }
 template<typename... Args>
 inline static std::string Log::Format(spdlog::string_view_t fmt, const Args&... args) {
  try {
   fmt::memory_buffer buf;
   fmt::format_to(buf, fmt, args...);
   return std::string(buf.data(), buf.size());
  }
  catch (const spdlog::spdlog_ex& ex) {
   std::cout << "Log initialization failed: " << ex.what() << std::endl;
   return "";
  }
 }


 class ISKLog {
 public:
  ISKLog() {
  }
  ~ISKLog() {
   UnInit();
  }
 public:
  void SetupSKLog(const std::string& path, const std::string& path2, const EnLogType& type, const tf_spdlog_output_cb& logmsgcb = nullptr) {
   if (m_IsSetupSKLog.load()) return;
   m_pLog = new Log(path, logmsgcb);
   m_LogKey = m_pLog->Alloc(path2, type);
   m_IsSetupSKLog.store(true);
  }

  void Init(const std::string& path, const std::string& path2, const EnLogType& type, const tf_spdlog_output_cb& logmsgcb = nullptr) {
   if (m_IsSetupSKLog.load()) return;
   m_pLog = new Log(path, logmsgcb);
   m_LogKey = m_pLog->Alloc(path2, type);
   m_IsSetupSKLog.store(true);
  }
  void UnInit() {
   if (m_IsSetupSKLog.load())
    if (m_pLog) {
     delete m_pLog;
     m_pLog = nullptr;
    }
   m_IsSetupSKLog.store(false);
  }
  void ExitSKLog() {
   UnInit();
  }
  template<typename... Args>
  inline void LOG(const EnLogType&, const unsigned int&, spdlog::string_view_t fmt, const Args&... args);
  template<typename... Args>
  inline void LOG_ERROR(spdlog::string_view_t fmt, const Args&... args);
  template<typename... Args>
  inline void LOG_INFO(spdlog::string_view_t fmt, const Args&... args);
  template<typename... Args>
  inline void LOG_TRACE(spdlog::string_view_t fmt, const Args&... args);
  template<typename... Args>
  inline void LOG_WARN(spdlog::string_view_t fmt, const Args&... args);
 private:
  std::string m_LogKey;
  Log* m_pLog = nullptr;
  std::string m_LogPath;
  std::atomic_bool m_IsSetupSKLog = false;
 };

 template<typename... Args>
 inline void ISKLog::LOG_ERROR(spdlog::string_view_t fmt, const Args&... args) {
  if (m_pLog && !m_LogKey.empty()) {
   fmt::memory_buffer buf;
   fmt::format_to(buf, fmt, args...);
   m_pLog->log(m_LogKey, EnLogType::EN_LOG_TYPE_SYSTEM, SPDLOG_LEVEL_ERROR, std::string(buf.data(), buf.size()));
  }
 }
 template<typename... Args>
 inline void ISKLog::LOG_INFO(spdlog::string_view_t fmt, const Args&... args) {
  if (m_pLog && !m_LogKey.empty()) {
   fmt::memory_buffer buf;
   fmt::format_to(buf, fmt, args...);
   m_pLog->log(m_LogKey, EnLogType::EN_LOG_TYPE_SYSTEM, SPDLOG_LEVEL_INFO, std::string(buf.data(), buf.size()));
  }
 }
 template<typename... Args>
 inline void ISKLog::LOG_TRACE(spdlog::string_view_t fmt, const Args&... args) {
  if (m_pLog && !m_LogKey.empty()) {
   fmt::memory_buffer buf;
   fmt::format_to(buf, fmt, args...);
   m_pLog->log(m_LogKey, EnLogType::EN_LOG_TYPE_TRACE, SPDLOG_LEVEL_INFO, std::string(buf.data(), buf.size()));
  }
 }
 template<typename... Args>
 inline void ISKLog::LOG_WARN(spdlog::string_view_t fmt, const Args&... args) {
  if (m_pLog && !m_LogKey.empty()) {
   fmt::memory_buffer buf;
   fmt::format_to(buf, fmt, args...);
   m_pLog->log(m_LogKey, EnLogType::EN_LOG_TYPE_SYSTEM, SPDLOG_LEVEL_WARN, std::string(buf.data(), buf.size()));
  }
 }
 template<typename... Args>
 inline void ISKLog::LOG(const EnLogType& type, const unsigned int& level, spdlog::string_view_t fmt, const Args&... args) {
  if (m_pLog && !m_LogKey.empty()) {
   fmt::memory_buffer buf;
   fmt::format_to(buf, fmt, args...);
   m_pLog->log(m_LogKey, type, level, std::string(buf.data(), buf.size()));
  }
 }
}///namespace spdlog

/// /*新生®（上海）**/
/// /*2022_09_26T22:41:11.7953709Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___586DABC5_153C_4C66_96BB_F260D1ADC3C8__HEAD__




