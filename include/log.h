#if !defined(INC_H___B597F7B9_E153_49DC_8C1A_C3C47D0BC0D0__HEAD__)
#define INC_H___B597F7B9_E153_49DC_8C1A_C3C47D0BC0D0__HEAD__


namespace shared {
	class ILog {
		std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
	public:
		ILog(const std::string& logPath, const std::string& moduleName) : \
			m_LogPath(logPath + R"(\logs\)"),
			m_ModuleName(moduleName + R"(.log)") {
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
/// /*2022_03_01T00:42:29.9421088Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___B597F7B9_E153_49DC_8C1A_C3C47D0BC0D0__HEAD__





