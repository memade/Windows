#if !defined(INC_H___B597F7B9_E153_49DC_8C1A_C3C47D0BC0D0__HEAD__)
#define INC_H___B597F7B9_E153_49DC_8C1A_C3C47D0BC0D0__HEAD__

#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <mutex>
#include <format>

namespace shared {
 class ILog {
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
 private:
  bool CreateDirectoryA(const std::string& create_directory_path) {
   bool result = false;
   do {
    if (create_directory_path.empty())
     break;
    std::string temp_directory_path{ create_directory_path };
    do {
     if (::_access(temp_directory_path.c_str(), 0) == 0) {
      result = true;//Directory already exists
      break;
     }
     for (auto it = temp_directory_path.begin(); it != temp_directory_path.end(); ++it) {
      if (*it == '/')
       *it = '\\';
     }
     do {
      auto found = temp_directory_path.find("\\\\");
      if (found == std::string::npos)
       break;
      temp_directory_path.replace(found, ::strlen("\\\\"), "\\");
     } while (1);

     std::vector<std::string> levels;
     do {
      if (::_access(temp_directory_path.c_str(), 0) == 0)
       break;
      if (temp_directory_path.empty())
       break;
      levels.emplace_back(temp_directory_path);
      temp_directory_path.pop_back();
      temp_directory_path = temp_directory_path.substr(0, temp_directory_path.rfind("\\"));
     } while (1);

     std::sort(levels.begin(), levels.end(),
      [&](const auto& obj1, const auto& obj2)->bool {
       return obj1.size() < obj2.size();
      });

     result = true;
     for (auto& level : levels) {
      if (::_mkdir(level.c_str()) != 0) {
       result = false;
       break;
      }
     }
    } while (0);
   } while (0);
   return result;
  }
  std::string TimeStringA(bool IsMilliseconds = true) const {
   std::string result;
   std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
   std::uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
    - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;
   std::time_t tt = std::chrono::system_clock::to_time_t(now);
   ::tm time_tm = { 0 };
   ::localtime_s(&time_tm, &tt);
   result = IsMilliseconds ? std::format(
    "{:04}/{:02}/{:02} {:02}:{:02}:{:02}.{:03}",
    time_tm.tm_year + 1900,
    time_tm.tm_mon + 1,
    time_tm.tm_mday,
    time_tm.tm_hour,
    time_tm.tm_min,
    time_tm.tm_sec,
    dis_millseconds
   ) : std::format(
    "{:04}/{:02}/{:02} {:02}:{:02}:{:02}",
    time_tm.tm_year + 1900,
    time_tm.tm_mon + 1,
    time_tm.tm_mday,
    time_tm.tm_hour,
    time_tm.tm_min,
    time_tm.tm_sec
   );
   return result;
  }
  bool Write(const std::string& WriteData) const {
   bool result = false;
   do {
    if (WriteData.empty())
     break;
    std::ofstream of(m_LogPathname, std::ios::binary | std::ios::out | std::ios::app);
    if (!of.is_open())
     break;
    of << TimeStringA(true);
    of << "  ";
    of << WriteData;
    of << std::endl;
    of.close();
    result = true;
   } while (0);
   return result;
  }
 public:
  template <class... _Types>
  _NODISCARD inline void Info(const std::string_view _Fmt, const _Types&... _Args) const {
   std::lock_guard<std::mutex> lock{ *m_Mutex };
   Write(_STD vformat(_Fmt, _STD make_format_args(_Args...)));
  }
#if 0 //!@ Not supported for now UNICODE.
  template <class... _Types>
  _NODISCARD inline void Info(const std::wstring_view _Fmt, const _Types&... _Args) const {
   std::lock_guard<std::mutex> lock{ *m_Mutex };
   do {
    std::string write = shared::IConv::WStringToMBytes(_STD vformat(_Fmt, _STD make_wformat_args(_Args...)));
    if (write.empty())
     break;
    write.insert(0, TimeStringA(true) + "\t");
    write.append(sizeof(char), '\n');
    shared::Win::File::Write(m_LogPath + m_ModuleName, write, std::ios::binary | std::ios::app);
   } while (0);
  }
#endif
 public:
  ILog(const std::string& logPath, const std::string& moduleName, const std::string& logFormat = R"(.log)") : \
   m_LogPath(logPath),
   m_ModuleName(moduleName) {
   m_LogPathname = m_LogPath + m_ModuleName + logFormat;
   CreateDirectoryA(m_LogPath);
  }
  ~ILog() {
  }
 private:
  std::string m_LogPath;
  std::string m_ModuleName;
  std::string m_LogPathname;
 };

}///namespace shared

/// /*新生®（上海）**/
/// /*2022_03_01T00:42:29.9421088Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___B597F7B9_E153_49DC_8C1A_C3C47D0BC0D0__HEAD__





