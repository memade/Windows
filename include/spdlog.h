#if !defined(INC_H___78761A34_5FC3_493C_84A3_0AA89C4F9809__HEAD__)
#define INC_H___78761A34_5FC3_493C_84A3_0AA89C4F9809__HEAD__

#define SPDLOG_USE_STD_FORMAT
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
	typedef enum class EnSpdlogLevelType : std::uint64_t {
		EN_SPDLOG_LEVEL_TRACE = 1,
		EN_SPDLOG_LEVEL_DEBUG = EN_SPDLOG_LEVEL_TRACE << 1,
		EN_SPDLOG_LEVEL_INFO = EN_SPDLOG_LEVEL_TRACE << 2,
		EN_SPDLOG_LEVEL_WARN = EN_SPDLOG_LEVEL_TRACE << 3,
		EN_SPDLOG_LEVEL_ERROR = EN_SPDLOG_LEVEL_TRACE << 4,
		EN_SPDLOG_LEVEL_CRITICAL = EN_SPDLOG_LEVEL_TRACE << 5,
		EN_SPDLOG_LEVEL_OFF = EN_SPDLOG_LEVEL_TRACE << 6,
		EN_SPDLOG_LEVEL_NORMAL = EN_SPDLOG_LEVEL_TRACE << 7,

		EN_SPDLOG_LEVEL_BEGIN = EN_SPDLOG_LEVEL_TRACE,
		EN_SPDLOG_LEVEL_END = EN_SPDLOG_LEVEL_NORMAL,
	}EnLogLevelType;

	const std::map<EnSpdlogLevelType, std::string> MapLogTypeIdentify = {
{EnSpdlogLevelType::EN_SPDLOG_LEVEL_TRACE ,"TRACE" },
{EnSpdlogLevelType::EN_SPDLOG_LEVEL_DEBUG ,"DEBUG" },
{EnSpdlogLevelType::EN_SPDLOG_LEVEL_INFO ,"INFO" },
{EnSpdlogLevelType::EN_SPDLOG_LEVEL_WARN ,"WARN" },
{EnSpdlogLevelType::EN_SPDLOG_LEVEL_ERROR ,"ERROR" },
{EnSpdlogLevelType::EN_SPDLOG_LEVEL_CRITICAL ,"CRITICAL" },
{EnSpdlogLevelType::EN_SPDLOG_LEVEL_OFF ,"OFF" },
{EnSpdlogLevelType::EN_SPDLOG_LEVEL_NORMAL ,"NORMAL" },
	};

	inline EnSpdlogLevelType operator|(const EnSpdlogLevelType& obj1, const unsigned long long& obj2) {
		return EnSpdlogLevelType(unsigned long long(obj1) | obj2);
	}
	inline EnSpdlogLevelType operator|(const EnSpdlogLevelType& obj1, const EnSpdlogLevelType& obj2) {
		return EnSpdlogLevelType(unsigned long long(obj1) | unsigned long long(obj2));
	}
	inline EnSpdlogLevelType operator|(const unsigned long long& obj1, const EnSpdlogLevelType& obj2) {
		return EnSpdlogLevelType(obj1 | unsigned long long(obj2));
	}
	inline bool operator&(const EnSpdlogLevelType& obj1, const EnSpdlogLevelType& obj2) {
		return unsigned long long(obj1) & unsigned long long(obj2);
	}

	class ISpdlog {
	protected:
		std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
	public:
		ISpdlog(
			const std::string& logPath,
			const std::string& moduleName,
			const EnLogLevelType& levelType = \
			shared::EnLogLevelType::EN_SPDLOG_LEVEL_INFO | \
			shared::EnLogLevelType::EN_SPDLOG_LEVEL_ERROR | shared::EnLogLevelType::EN_SPDLOG_LEVEL_WARN) :
			m_LogPath(logPath + R"(\logs\)" + moduleName + "\\"),
			m_LevelType(levelType) {
			Init();
		}
		~ISpdlog() {
			UnInit();
		}
	public:
		template <class... _Types>
		_NODISCARD inline void Normal(const std::string_view _Fmt, const _Types&... _Args) const;
		template <class... _Types>
		_NODISCARD inline void Normal(const std::wstring_view _Fmt, const _Types&... _Args) const;
		template <class... _Types>
		_NODISCARD inline void Info(const std::string_view _Fmt, const _Types&... _Args) const;
		template <class... _Types>
		_NODISCARD inline void Info(const std::wstring_view _Fmt, const _Types&... _Args) const;
		template <class... _Types>
		_NODISCARD inline void Error(const std::string_view _Fmt, const _Types&... _Args) const;
		template <class... _Types>
		_NODISCARD inline void Error(const std::wstring_view _Fmt, const _Types&... _Args) const;
		template <class... _Types>
		_NODISCARD inline void Warn(const std::string_view _Fmt, const _Types&... _Args) const;
		template <class... _Types>
		_NODISCARD inline void Warn(const std::wstring_view _Fmt, const _Types&... _Args) const;
#if 0
		template <class... _Types>
		inline void Error(const std::string_view _Fmt, const _Types&... _Args) const;
		template <class... _Types>
		inline void Warn(const std::string_view _Fmt, const _Types&... _Args) const;
#endif
	protected:
		inline void Init();
		inline void UnInit();
		inline bool Alloc(const EnLogLevelType&);
	protected:
		const std::string m_LogPath;
		const std::string m_LogFileSuffix = ".log";
		const EnLogLevelType m_LevelType;
		std::map<EnLogLevelType, std::shared_ptr<spdlog::logger>> m_Loggers;
	};

	inline void ISpdlog::Init() {
		do {
			if (std::uint64_t(m_LevelType) <= 0)
				break;
			if (m_LevelType & EnLogLevelType::EN_SPDLOG_LEVEL_INFO) {
				Alloc(EnLogLevelType::EN_SPDLOG_LEVEL_INFO);
				Info("{}", std::string((char*)&__XSLC_TEAM_TENET_[0],sizeof(__XSLC_TEAM_TENET_)));
			}


			if (m_LevelType & EnLogLevelType::EN_SPDLOG_LEVEL_WARN)
				Alloc(EnLogLevelType::EN_SPDLOG_LEVEL_WARN);

			if (m_LevelType & EnLogLevelType::EN_SPDLOG_LEVEL_ERROR)
				Alloc(EnLogLevelType::EN_SPDLOG_LEVEL_ERROR);

			if (m_LevelType & EnLogLevelType::EN_SPDLOG_LEVEL_DEBUG)
				Alloc(EnLogLevelType::EN_SPDLOG_LEVEL_DEBUG);

			if (m_LevelType & EnLogLevelType::EN_SPDLOG_LEVEL_CRITICAL)
				Alloc(EnLogLevelType::EN_SPDLOG_LEVEL_CRITICAL);

			if (m_LevelType & EnLogLevelType::EN_SPDLOG_LEVEL_TRACE)
				Alloc(EnLogLevelType::EN_SPDLOG_LEVEL_TRACE);

			if (m_LevelType & EnLogLevelType::EN_SPDLOG_LEVEL_OFF)
				Alloc(EnLogLevelType::EN_SPDLOG_LEVEL_OFF);

			if (m_LevelType & EnLogLevelType::EN_SPDLOG_LEVEL_NORMAL)
				Alloc(EnLogLevelType::EN_SPDLOG_LEVEL_NORMAL);

		} while (0);
	}

	inline void ISpdlog::UnInit() {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		Info("{}", std::string((char*)&__XSLC_TEAM_SIGN_[0], sizeof(__XSLC_TEAM_SIGN_)));
		spdlog::flush_every(std::chrono::seconds(30));
		spdlog::shutdown();
		spdlog::drop_all();
	}

	inline bool ISpdlog::Alloc(const EnLogLevelType& type) {
		bool result = false;
		do {
			if (type< EnLogLevelType::EN_SPDLOG_LEVEL_BEGIN || type> EnLogLevelType::EN_SPDLOG_LEVEL_END)
				break;
			auto findTypeString = MapLogTypeIdentify.find(type);
			if (findTypeString == MapLogTypeIdentify.end())
				break;
			std::string identify = m_LogPath + R"(\)" + findTypeString->second + m_LogFileSuffix;
			std::vector<spdlog::sink_ptr> sinks;
			sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(identify, 0, 0));
			auto pLogger = std::make_shared<spdlog::logger>(identify, begin(sinks), end(sinks));
#ifdef _DEBUG //! 设置日志记录级别
			pLogger->set_level(spdlog::level::trace);
#else
			pLogger->set_level(spdlog::level::info);
#endif
			pLogger->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%t][%L] %^%v%$");
			/*spLogger->set_pattern("[%Y%m%d %H:%M:%S.%e][%t][%L][%n] %^%v%$");*/
			pLogger->flush_on(spdlog::level::trace);
			auto found = m_Loggers.find(type);
			if (found != m_Loggers.end())
				m_Loggers.erase(found);
			m_Loggers.emplace(type, pLogger);
			result = true;
		} while (0);
		return result;
	}

	template <class... _Types>
	_NODISCARD inline void ISpdlog::Normal(const std::string_view _Fmt, const _Types&... _Args) const {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		do {
			auto found = m_Loggers.find(EnLogLevelType::EN_SPDLOG_LEVEL_NORMAL);
			if (m_Loggers.end() == found)
				break;
			found->second->info(_STD vformat(_Fmt, _STD make_format_args(_Args...)));
		} while (0);
	}
	template <class... _Types>
	_NODISCARD inline void ISpdlog::Normal(const std::wstring_view _Fmt, const _Types&... _Args) const {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		do {
			auto found = m_Loggers.find(EnLogLevelType::EN_SPDLOG_LEVEL_NORMAL);
			if (m_Loggers.end() == found)
				break;
			found->second->info(_STD vformat(_Fmt, _STD make_wformat_args(_Args...)));
		} while (0);
	}


	template <class... _Types>
	_NODISCARD inline void ISpdlog::Info(const std::string_view _Fmt, const _Types&... _Args) const {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		do {
			auto found = m_Loggers.find(EnLogLevelType::EN_SPDLOG_LEVEL_INFO);
			if (m_Loggers.end() == found)
				break;
			found->second->info(_STD vformat(_Fmt, _STD make_format_args(_Args...)));
		} while (0);
	}

	template <class... _Types>
	_NODISCARD inline void ISpdlog::Info(const std::wstring_view _Fmt, const _Types&... _Args) const {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		do {
			auto found = m_Loggers.find(EnLogLevelType::EN_SPDLOG_LEVEL_INFO);
			if (m_Loggers.end() == found)
				break;
			found->second->info(_STD vformat(_Fmt, _STD make_wformat_args(_Args...)));
		} while (0);
	}

	template <class... _Types>
	_NODISCARD inline void ISpdlog::Error(const std::string_view _Fmt, const _Types&... _Args) const {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		do {
			auto found = m_Loggers.find(EnLogLevelType::EN_SPDLOG_LEVEL_INFO);
			if (m_Loggers.end() == found)
				break;
			found->second->error(_STD vformat(_Fmt, _STD make_format_args(_Args...)));
		} while (0);
	}

	template <class... _Types>
	_NODISCARD inline void ISpdlog::Error(const std::wstring_view _Fmt, const _Types&... _Args) const {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		do {
			auto found = m_Loggers.find(EnLogLevelType::EN_SPDLOG_LEVEL_INFO);
			if (m_Loggers.end() == found)
				break;
			found->second->error(_STD vformat(_Fmt, _STD make_wformat_args(_Args...)));
		} while (0);
	}


	template <class... _Types>
	_NODISCARD inline void ISpdlog::Warn(const std::string_view _Fmt, const _Types&... _Args) const {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		do {
			auto found = m_Loggers.find(EnLogLevelType::EN_SPDLOG_LEVEL_INFO);
			if (m_Loggers.end() == found)
				break;
			found->second->warn(_STD vformat(_Fmt, _STD make_format_args(_Args...)));
		} while (0);
	}

	template <class... _Types>
	_NODISCARD inline void ISpdlog::Warn(const std::wstring_view _Fmt, const _Types&... _Args) const {
		std::lock_guard<std::mutex> lock{ *m_Mutex };
		do {
			auto found = m_Loggers.find(EnLogLevelType::EN_SPDLOG_LEVEL_INFO);
			if (m_Loggers.end() == found)
				break;
			found->second->warn(_STD vformat(_Fmt, _STD make_wformat_args(_Args...)));
		} while (0);
	}

	class ILog {
		std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
	public:
		ILog(const std::string& logPath, const std::string& moduleName) : \
			m_LogPath(logPath + R"(\logs\)"),
			m_ModuleName(moduleName+R"(.log)") {
		}
		~ILog() {

		}
	public:
		template <class... _Types>
		_NODISCARD inline void Info(const std::string_view _Fmt, const _Types&... _Args) const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			do {
				std::string write = _STD vformat(_Fmt, _STD make_format_args(_Args...));
				if (write.empty())
					break;
				write.insert(0, shared::Win::Time::TimeSystemString(true) + "\t");
				write.append(sizeof(char), '\n');
				shared::Win::File::Write(m_LogPath + m_ModuleName, write, std::ios::binary | std::ios::app);
			} while (0);
		}

		template <class... _Types>
		_NODISCARD inline void Info(const std::wstring_view _Fmt, const _Types&... _Args) const {
			std::lock_guard<std::mutex> lock{ *m_Mutex };
			do {
				std::string write = shared::IConv::WStringToMBytes(_STD vformat(_Fmt, _STD make_wformat_args(_Args...)));
				if (write.empty())
					break;
				write.insert(0, shared::Win::Time::TimeSystemString(true) + "\t");
				write.append(sizeof(char), '\n');
				shared::Win::File::Write(m_LogPath + m_ModuleName, write, std::ios::binary | std::ios::app);
			} while (0);
		}
	private:
		std::string m_LogPath;
		std::string m_ModuleName;
	};



































}///namespace shared



/// /*新生®（上海）**/
/// /*2022_02_21T03:51:51.9583577Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___78761A34_5FC3_493C_84A3_0AA89C4F9809__HEAD__




