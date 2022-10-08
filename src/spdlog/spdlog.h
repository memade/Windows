#if !defined(INC_H___35279C84_C66C_4274_861F_9230C8CD93A3__HEAD__)
#define INC_H___35279C84_C66C_4274_861F_9230C8CD93A3__HEAD__

namespace local {

 class Log final {
 public:
  Log(const std::string& path, spdlog::tf_spdlog_output_cb outputcb = nullptr) : m_LogPathBase(path + "/logs/"), m_outputcb(outputcb) {}
  ~Log() { Free(); }
  std::shared_ptr<spdlog::logger>
   log(const std::string& process,
    shared::EnLogType logType = shared::EnLogType::EN_LOG_TYPE_SYSTEM) {
   return m_pSpdlogPtrQ[process][logType];
  }

  void log(const std::string& key, const shared::EnLogType& type, const unsigned int& level, const std::string& logdata) {
   if (key.empty() || logdata.empty())
    return;
   m_pSpdlogPtrQ.search(
    key,
    [&](const auto&, auto& loggers)
    {
     loggers.search(
      type,
      [&](const shared::EnLogType&, std::shared_ptr<spdlog::logger>& logger)
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

  std::string Alloc(const std::string& process_name, const shared::EnLogType& LogTypes) {
   auto process = process_name.substr(0, process_name.rfind("."));
   //!@ 移除空格
   for (auto it = process.begin(); it != process.end();) {
    if (*it == ' ')
     it = process.erase(it);
    else
     ++it;
   }
   std::string module_name;
   shared::container::map<shared::EnLogType, std::shared_ptr<spdlog::logger>> processLoggers;

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
    processLoggers.push(shared::EnLogType::EN_LOG_TYPE_SYSTEM, result);
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
    auto new_formatter = spdlog::details::make_unique<spdlog::pattern_formatter>("%^%v%$", spdlog::pattern_time_type::local, "");
    result->set_formatter(std::move(new_formatter));

    //result->set_pattern("%^%v%$");
    result->flush_on(spdlog::level::trace);
    processLoggers.push(shared::EnLogType::EN_LOG_TYPE_NORMAL, result);
   }

   if (LogTypes & shared::EnLogType::EN_LOG_TYPE_ERROR)
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
    processLoggers.push(shared::EnLogType::EN_LOG_TYPE_ERROR, result);
   }
   if (LogTypes & shared::EnLogType::EN_LOG_TYPE_WARN)
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
    processLoggers.push(shared::EnLogType::EN_LOG_TYPE_WARN, result);
   }
   if (LogTypes & shared::EnLogType::EN_LOG_TYPE_TRACE)
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
    processLoggers.push(shared::EnLogType::EN_LOG_TYPE_TRACE, result);
   }
   if (LogTypes & shared::EnLogType::EN_LOG_TYPE_SUCCESS)
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
    processLoggers.push(shared::EnLogType::EN_LOG_TYPE_SUCCESS, result);
   }
   if (LogTypes & shared::EnLogType::EN_LOG_TYPE_FAILED)
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
    processLoggers.push(shared::EnLogType::EN_LOG_TYPE_FAILED, result);
   }
   if (LogTypes & shared::EnLogType::EN_LOG_TYPE_SIGNAL)
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
    processLoggers.push(shared::EnLogType::EN_LOG_TYPE_SIGNAL, result);
   }
   if (LogTypes & shared::EnLogType::EN_LOG_TYPE_FOLLOW)
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
    processLoggers.push(shared::EnLogType::EN_LOG_TYPE_FOLLOW, result);
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
     modules[shared::EnLogType::EN_LOG_TYPE_SYSTEM]->info(__XSLC_TEAM_TENET_);
    });
  }
  void Free()
  {
   m_pSpdlogPtrQ.iterate_clear(
    [&](const auto& process, auto& modules, auto& itbreak, auto& itclear)
    {
     modules[shared::EnLogType::EN_LOG_TYPE_SYSTEM]->info(__XSLC_TEAM_TENET_);
     itclear = true;
    });
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
   spdlog::flush_every(std::chrono::seconds(30));
   spdlog::shutdown();
   spdlog::drop_all();
  }
 public:
  const std::string& LogPath() const { return m_LogPathBase; }
 private:
  std::string m_LogPathBase;
  spdlog::tf_spdlog_output_cb m_outputcb = nullptr;
  shared::container::map<std::string, shared::container::map<shared::EnLogType, std::shared_ptr<spdlog::logger>>> m_pSpdlogPtrQ;
 };

 class Spdlog final : virtual public ISpdlog {
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
 public:
  Spdlog(const std::string&, const std::string&, const EnLogType&, const std::string&,const spdlog::tf_spdlog_output_cb& logmsgcb = nullptr);
  ~Spdlog();
 private:
  void Init();
  void UnInit();
  void CreateLogger(const EnLogType&,const std::string&);
 protected:
  void Release() const override final;
  void LOG(const std::string&, const EnLogType&) override final;
  void LOG(const std::wstring&, const EnLogType&) override final;
 private:
  std::string m_LogFilePath;
  const std::string m_LogFileFormat;
  const std::string m_ModuleName;
  const std::string m_LogsPath;
  const EnLogType m_LogTypes;
  const spdlog::tf_spdlog_output_cb m_spdlog_output_cb;
 private:
  std::map<shared::EnLogType, spdlog::logger*> m_LoggerMapPtr;
 };

#if 0
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
#endif


}///namespace local


/// /*新生®（上海）**/
/// /*2022_10_08T05:24:20.0490725Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___35279C84_C66C_4274_861F_9230C8CD93A3__HEAD__