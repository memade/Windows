#include "stdafx.h"

namespace shared {
 static const int RFC1123_TIME_LEN = 29;
 static const char* DAY_NAMES[] =
 { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
 static const char* MONTH_NAMES[] =
 { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
 std::string Win::Time::RFC1123() {
  /*InternetTimeFromSystemTime()*/
  std::string result;
  time_t t;
  struct tm tm;
  result.resize(RFC1123_TIME_LEN, 0x00);
  ::time(&t);
  ::gmtime_s(&tm, &t);
  ::strftime(&result[0], RFC1123_TIME_LEN + 1, "---, %d --- %Y %H:%M:%S GMT", &tm);
  ::memcpy(&result[0], DAY_NAMES[tm.tm_wday], 3);
  ::memcpy(&result[0] + 8, MONTH_NAMES[tm.tm_mon], 3);
  return result;
 }
 std::string Win::Time::GMTTimeStringGet() {
  auto now = std::chrono::system_clock::now();
  auto itt = std::chrono::system_clock::to_time_t(now);
  std::ostringstream ss;
  ss << std::put_time(gmtime(&itt), R"(%FT%TZ)");
  return ss.str();
 }
#if 0
 template<typename T /*= std::chrono::seconds*/>
 std::time_t Win::Time::TimeStamp() {
  return std::chrono::duration_cast<T>(std::chrono::time_point_cast<T>(std::chrono::system_clock::now()).time_since_epoch()).count();
 }
#endif

 std::time_t Win::Time::TimeStampWebkit() {
  return TimeStamp<std::chrono::microseconds>() + 11644473600000000LL;
 }
 std::time_t Win::Time::TimestampMillisecondsToSeconds(const std::time_t& milliseconds) {
  std::time_t result = 0;
  do {
   if (milliseconds <= 0)
    break;
#if 0
   //!@ 最大秒级时间戳
   const auto s_max = 9999999999;//1647396747;
   //!@ 最大毫秒级时间戳 
   const auto ms_max = 9999999999999;
#endif
   result = (time_t)((milliseconds - (milliseconds % 1000)) / 1000);
  } while (0);
  return result;
 }
 std::string Win::Time::TimeSystemString(bool IsMilliseconds /*= false*/) {
  std::string result;
#if 0
  SYSTEMTIME _sysTime;
  ::GetLocalTime(&_sysTime);
  if (IsMilliseconds) {
   result = std::format("{:04}/{:02}/{:02} {:02}:{:02}:{:02}:{:03}",
    _sysTime.wYear,
    _sysTime.wMonth,
    _sysTime.wDay,
    _sysTime.wHour,
    _sysTime.wMinute,
    _sysTime.wSecond,
    _sysTime.wMilliseconds);
  }
  else {
   result = std::format("{:04}/{:02}/{:02} {:02}:{:02}:{:02}",
    _sysTime.wYear,
    _sysTime.wMonth,
    _sysTime.wDay,
    _sysTime.wHour,
    _sysTime.wMinute,
    _sysTime.wSecond);
  }
#else
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
   - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;
  std::time_t tt = std::chrono::system_clock::to_time_t(now);
  tm time_tm = { 0 };
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
#endif
  return result;
 }

 UINT Win::Time::TimePeriodMade(const UINT& Hour, const UINT& Minute, const UINT& Second, bool end /*= false*/)
 {
  UINT result = 0;
  result = Hour * 60 * 60 + Minute * 60 + Second;
  if (end && !result)
   result = 90060;
  return result;
 }
 std::string Win::Time::TimePeriodUnMade(const time_t& TimePeriod) {
  return TimePeriodUnMade(static_cast<UINT>(TimePeriod));
 }
 std::string Win::Time::TimePeriodUnMade(const unsigned long long& TimePeriod) {
  return TimePeriodUnMade(static_cast<UINT>(TimePeriod));
 }
 std::string Win::Time::TimePeriodUnMade(const UINT& TimePeriod)
 {
  std::string result = "00:00:00";
  if (TimePeriod < 90060 && TimePeriod>0)
  {
   std::int64_t sec = TimePeriod % 60;
   std::int64_t min = (TimePeriod - sec) % 3600 / 60;
   std::int64_t hour = (TimePeriod - sec - min * 60) / 3600;
   if (sec >= 0 && sec <= 59 && min >= 0 && min <= 59 && hour >= 0 && hour <= 23)
   {
    result.clear();
    char stime[64] = { 0 };
    auto len = sprintf_s(stime, "%02I64d:%02I64d:%02I64d", hour, min, sec);
    result.append(stime, len);
   }
  }
  return result;
 }
}///namespace 