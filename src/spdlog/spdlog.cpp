#include "stdafx.h"

shared::ISpdlog* __gpSpdlog = nullptr;

namespace local {
 Spdlog::Spdlog(const std::string& module_name, const std::string& logs_path, const EnLogType& logTypes, const std::string& logFileFormat, const spdlog::tf_spdlog_output_cb& logmsgcb /*= nullptr*/) :
  m_LogFileFormat(logFileFormat),
  m_spdlog_output_cb(logmsgcb),
  m_LogTypes(logTypes),
  m_ModuleName(module_name),
  m_LogsPath(logs_path) {
  Init();
 }
 Spdlog::~Spdlog() {
  UnInit();
 }
 void Spdlog::Release() const {
  delete this;
 }
 void Spdlog::Init() {
  m_LoggerMapPtr.clear();

  m_LogFilePath = shared::Win::PathFixedA(m_LogsPath + "\\" + m_ModuleName + "\\");
  if (!shared::Win::AccessA(m_LogFilePath))
   assert(shared::Win::CreateDirectoryA(m_LogFilePath));

  if (m_LogTypes & EnLogType::EN_LOG_TYPE_TRACE) {
   CreateLogger(EnLogType::EN_LOG_TYPE_TRACE, "trace");
  }
  if (m_LogTypes & EnLogType::EN_LOG_TYPE_ERROR) {
   CreateLogger(EnLogType::EN_LOG_TYPE_ERROR, "error");
  }
  if (m_LogTypes & EnLogType::EN_LOG_TYPE_WARN) {
   CreateLogger(EnLogType::EN_LOG_TYPE_WARN, "warn");
  }
  if (m_LogTypes & EnLogType::EN_LOG_TYPE_FAILED) {
   CreateLogger(EnLogType::EN_LOG_TYPE_FAILED, "failed");
  }
  if (m_LogTypes & EnLogType::EN_LOG_TYPE_SUCCESS) {
   CreateLogger(EnLogType::EN_LOG_TYPE_SUCCESS, "success");
  }
  if (m_LogTypes & EnLogType::EN_LOG_TYPE_SIGNAL) {
   CreateLogger(EnLogType::EN_LOG_TYPE_SIGNAL, "signal");
  }
  if (m_LogTypes & EnLogType::EN_LOG_TYPE_FOLLOW) {
   CreateLogger(EnLogType::EN_LOG_TYPE_FOLLOW, "follow");
  }
  if (m_LogTypes & EnLogType::EN_LOG_TYPE_SYSTEM) {
   CreateLogger(EnLogType::EN_LOG_TYPE_SYSTEM, "system");
  }
  if (m_LogTypes & EnLogType::EN_LOG_TYPE_NORMAL) {
   CreateLogger(EnLogType::EN_LOG_TYPE_NORMAL, "normal");
  }
 }
 void Spdlog::UnInit() {
  for (auto& node : m_LoggerMapPtr) {
   if (node.first == EnLogType::EN_LOG_TYPE_SYSTEM)
    node.second->info((char*)&__XSLC_TEAM_SIGN_[0]);
   SK_DELETE_PTR(node.second);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  spdlog::flush_every(std::chrono::seconds(30));
  spdlog::shutdown();
  spdlog::drop_all();
  m_LoggerMapPtr.clear();
 }
 void Spdlog::CreateLogger(const EnLogType& logType, const std::string& logger_key) {
  spdlog::logger* pLogger = nullptr;
  do {
   if (logger_key.empty())
    break;
   if (m_LoggerMapPtr.find(logType) != m_LoggerMapPtr.end())
    break;
   if (logType & EnLogType::EN_LOG_TYPE_NORMAL) {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>
     (
      shared::Win::PathFixedA(m_LogFilePath + "\\" + logger_key + m_LogFileFormat),
      0,
      0)
    );
    pLogger = new spdlog::logger(logger_key, begin(sinks), end(sinks));
    pLogger->set_level(spdlog::level::trace);
    auto new_formatter = spdlog::details::make_unique<spdlog::pattern_formatter>("%^%v%$", spdlog::pattern_time_type::local, "");
    pLogger->set_formatter(std::move(new_formatter));
    pLogger->flush_on(spdlog::level::trace);
    break;
   }

   std::vector<spdlog::sink_ptr> sinks;
   sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
   sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>
    (
     shared::Win::PathFixedA(m_LogFilePath + "\\" + logger_key + m_LogFileFormat),
     0,
     0)
   );
   pLogger = new spdlog::logger(logger_key, begin(sinks), end(sinks));
   pLogger->set_level(spdlog::level::trace);
   pLogger->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");
   /*spLogger->set_pattern("[%Y%m%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");*/
   pLogger->flush_on(spdlog::level::trace);
  } while (0);
  if (pLogger) {
   if (logType == EnLogType::EN_LOG_TYPE_SYSTEM)
    pLogger->info((char*)&__XSLC_TEAM_TENET_[0]);
   pLogger->set_output_cb(m_spdlog_output_cb);
   m_LoggerMapPtr.emplace(logType, pLogger);
  }
 }

 void Spdlog::LOG(const std::string& logData, const EnLogType& logType) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (logData.empty())
    break;
#if 0
   auto found = m_LoggerMapPtr.find(logType);
   if (found == m_LoggerMapPtr.end())
    break;
   switch (found->first) {
   case EnLogType::EN_LOG_TYPE_TRACE: {
    found->second->trace(logData);
   }break;
   case EnLogType::EN_LOG_TYPE_ERROR:
    [[fallthrough]];
   case EnLogType::EN_LOG_TYPE_FAILED: {
    found->second->error(logData);
   }break;
   case EnLogType::EN_LOG_TYPE_WARN: {
    found->second->warn(logData);
   }break;
   default:
    found->second->info(logData);
    break;
   }
#endif
   spdlog::logger* log = nullptr;
   auto found = m_LoggerMapPtr.find(logType);
   if (found == m_LoggerMapPtr.end()) {
    if(m_LoggerMapPtr.empty())
     break;
    log = m_LoggerMapPtr.begin()->second;
   }
   else {
    log = found->second;
   }
   if (!log)
    break;
   switch (logType) {
   case EnLogType::EN_LOG_TYPE_TRACE: {
    log->trace(logData);
   }break;
   case EnLogType::EN_LOG_TYPE_ERROR:
    [[fallthrough]];
   case EnLogType::EN_LOG_TYPE_FAILED: {
    log->error(logData);
   }break;
   case EnLogType::EN_LOG_TYPE_WARN: {
    log->warn(logData);
   }break;
   default:
    log->info(logData);
    break;
   }
  } while (0);
 }
 void Spdlog::LOG(const std::wstring& logData, const EnLogType& logType) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (logData.empty())
    break;
#if 0
   auto found = m_LoggerMapPtr.find(logType);
   if (found == m_LoggerMapPtr.end())
    break;
   switch (found->first) {
   case EnLogType::EN_LOG_TYPE_TRACE: {
    found->second->trace(logData);
   }break;
   case EnLogType::EN_LOG_TYPE_ERROR:
    [[fallthrough]];
   case EnLogType::EN_LOG_TYPE_FAILED: {
    found->second->error(logData);
   }break;
   case EnLogType::EN_LOG_TYPE_WARN: {
    found->second->warn(logData);
   }break;
   default:
    found->second->info(logData);
    break;
   }
#endif
   spdlog::logger* log = nullptr;
   auto found = m_LoggerMapPtr.find(logType);
   if (found == m_LoggerMapPtr.end()) {
    if (m_LoggerMapPtr.empty())
     break;
    log = m_LoggerMapPtr.begin()->second;
   }
   else {
    log = found->second;
   }
   if (!log)
    break;
   switch (logType) {
   case EnLogType::EN_LOG_TYPE_TRACE: {
    log->trace(logData);
   }break;
   case EnLogType::EN_LOG_TYPE_ERROR:
    [[fallthrough]];
   case EnLogType::EN_LOG_TYPE_FAILED: {
    log->error(logData);
   }break;
   case EnLogType::EN_LOG_TYPE_WARN: {
    log->warn(logData);
   }break;
   default:
    log->info(logData);
    break;
   }
  } while (0);
 }
}///namespace local

namespace shared {
 EnLogType operator|(const EnLogType& obj1, const unsigned long long& obj2) {
  return EnLogType(unsigned long long(obj1) | obj2);
 }
 EnLogType operator|(const EnLogType& obj1, const EnLogType& obj2) {
  return EnLogType(unsigned long long(obj1) | unsigned long long(obj2));
 }
 EnLogType operator|(const unsigned long long& obj1, const EnLogType& obj2) {
  return EnLogType(obj1 | unsigned long long(obj2));
 }
 bool operator&(const EnLogType& obj1, const EnLogType& obj2) {
  return unsigned long long(obj1) & unsigned long long(obj2);
 }
 void operator|=(EnLogType& obj1, const EnLogType& obj2) {
  obj1 = (obj1 | obj2);
 }
 EnLogType ParseLogTypes(const EnLogType& logTypes) {
  EnLogType result = EnLogType::EN_LOG_TYPE_UNDEFINED;
  if (logTypes & EnLogType::EN_LOG_TYPE_TRACE)
   result |= EnLogType::EN_LOG_TYPE_TRACE;
  if (logTypes & EnLogType::EN_LOG_TYPE_ERROR)
   result |= EnLogType::EN_LOG_TYPE_ERROR;
  if (logTypes & EnLogType::EN_LOG_TYPE_WARN)
   result |= EnLogType::EN_LOG_TYPE_WARN;
  if (logTypes & EnLogType::EN_LOG_TYPE_FAILED)
   result |= EnLogType::EN_LOG_TYPE_FAILED;
  if (logTypes & EnLogType::EN_LOG_TYPE_SUCCESS)
   result |= EnLogType::EN_LOG_TYPE_SUCCESS;
  if (logTypes & EnLogType::EN_LOG_TYPE_SIGNAL)
   result |= EnLogType::EN_LOG_TYPE_SIGNAL;
  if (logTypes & EnLogType::EN_LOG_TYPE_FOLLOW)
   result |= EnLogType::EN_LOG_TYPE_FOLLOW;
  if (logTypes & EnLogType::EN_LOG_TYPE_SYSTEM)
   result |= EnLogType::EN_LOG_TYPE_SYSTEM;
  //!@ 默认添加[系统级]日志
  else
   result |= EnLogType::EN_LOG_TYPE_SYSTEM;
  if (logTypes & EnLogType::EN_LOG_TYPE_NORMAL)
   result |= EnLogType::EN_LOG_TYPE_NORMAL;
  return result;
 }

 ISpdlog* ISpdlog::CreateInterface(
  const std::string& module_name,
  const std::string& logs_path,
  const EnLogType& logTypes /*= EnLogType::EN_LOG_TYPE_SYSTEM*/,
  const std::string& logFileFormat /*= R"(.log)"*/,
  const spdlog::tf_spdlog_output_cb& spdlog_output_cb /*= nullptr*/) {
  do {
   if (module_name.empty())
    break;
   std::string final_module_name{ module_name };
   for (auto it = final_module_name.begin(); it != final_module_name.end();++it) {
    if (*it == '.')
     *it = '_';
   }
   if (logs_path.empty())
    break;
   if (!shared::Win::AccessA(logs_path))
    if (!shared::Win::CreateDirectoryA(logs_path))
     break;
   if (EnLogType::EN_LOG_TYPE_UNDEFINED == ParseLogTypes(logTypes))
    break;
   __gpSpdlog = dynamic_cast<ISpdlog*>(new local::Spdlog(final_module_name, logs_path, logTypes, logFileFormat, spdlog_output_cb));
  } while (0);
  return __gpSpdlog;
 }
 void ISpdlog::DestoryInterface(ISpdlog*& spdlogObj) {
  do {
   if (!spdlogObj)
    break;
   spdlogObj->Release();
   spdlogObj = nullptr;
  } while (0);
 }
 void ISpdlog::DestoryInterface() {
  do {
   if (!__gpSpdlog)
    break;
   __gpSpdlog->Release();
   __gpSpdlog = nullptr;
  } while (0);
 }
}///namespace shared