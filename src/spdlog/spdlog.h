#if !defined(INC_H___35279C84_C66C_4274_861F_9230C8CD93A3__HEAD__)
#define INC_H___35279C84_C66C_4274_861F_9230C8CD93A3__HEAD__

namespace local {
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
}///namespace local


/// /*新生®（上海）**/
/// /*2022_10_08T05:24:20.0490725Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___35279C84_C66C_4274_861F_9230C8CD93A3__HEAD__