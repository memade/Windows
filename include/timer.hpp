#if !defined(INC_H___9B4F0A2C_6D72_4637_BAEA_91E3139C6F3A__HEAD__)
#define INC_H___9B4F0A2C_6D72_4637_BAEA_91E3139C6F3A__HEAD__

namespace shared {
	class Timer final {
	public:
		inline Timer();
		inline ~Timer();
	public:
		inline int Open();
		inline int Close();
		inline void TimerAppend(const __int64& _time_interval, void* _host_ptr, const std::function<void(const __int64&, const std::intptr_t&, const __int64&)>& _cb_timer);
		inline void TimerRemove(const __int64& _time_interval, void* _host_ptr);
	private:
		std::vector<std::thread> m_Threads;
		std::atomic<bool> m_IsOpen = false;
		std::atomic<bool> m_IsOpenT = true;
		container::map<__int64, __int64> m_TimestampBakQ;
		container::map<__int64, std::map<std::intptr_t, std::function<void(const __int64&, const std::intptr_t&, const __int64&)>>> m_TimerQ;
	};
	/// <summary>
	/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// </summary>
	inline Timer::Timer() {

	}
	inline Timer::~Timer() {

	}
	inline int Timer::Open()
	{
		if (m_IsOpen.load()) { return 0; }
		m_Threads.emplace_back(
			[&]() {
				do {
					m_TimerQ.iterate(
						[&](auto& key, auto& _map_business_cb) {
							m_TimestampBakQ.iterate(
								[&](const auto& _time_interval, auto& _timestamp_bak) {
									if (Win::Time::TimeStamp<std::chrono::milliseconds>() - _timestamp_bak >= _time_interval) {//! 允许10毫秒偏差
										for (const auto& node : _map_business_cb) {
											node.second(_time_interval, node.first, Win::Time::TimeStamp<std::chrono::milliseconds>());
										}
										_timestamp_bak = Win::Time::TimeStamp<std::chrono::milliseconds>();
									}
								});
						});
					if (!m_IsOpenT.load()) {
						break;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
				} while (1);
			});
		m_IsOpen.store(true);
		return 0;
	}

	inline int Timer::Close() {
		if (!m_IsOpen.load()) { return 0; }
		container::queue<__int64> _removes;
		m_TimerQ.iterate(
			[&](const auto& key, auto& val) {
				_removes.push(key);
			});
		if (!_removes.empty()) {
			auto node = _removes.pop();
			if (node) {
				m_TimerQ.pop(*node, [](const auto&, auto&) {});
				m_TimestampBakQ.pop(*node, [&](const auto&, auto&) {});
			}
		}
		m_IsOpenT.store(false);
		for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
			it->join();
		}
		m_Threads.clear();
		m_IsOpen.store(false);
		return 0;
	}
	inline void Timer::TimerAppend(const __int64& _time_interval, void* _host_ptr, const std::function<void(const __int64&, const std::intptr_t&, const __int64&)>& _cb_timer) {
		if (!m_TimerQ.search(_time_interval,
			[&](const auto& key, auto& val) {
				val.insert({ { (std::intptr_t)_host_ptr,_cb_timer } });
			})) {
			std::map<std::intptr_t, std::function<void(const __int64&, const std::intptr_t&, const __int64&)>> __map({ {(std::intptr_t)_host_ptr,_cb_timer } });
			m_TimerQ.push(_time_interval, __map);
			m_TimestampBakQ.push(_time_interval, Win::Time::TimeStamp<std::chrono::milliseconds>());
		}
	}

	inline void Timer::TimerRemove(const __int64& _time_interval, void* _host_ptr) {
		container::queue<__int64> _removes;
		m_TimerQ.iterate(
			[&](const auto& key, auto& val) {
				if (key == _time_interval) {
					auto ifind = val.find(reinterpret_cast<std::intptr_t>(_host_ptr));
					if (ifind != val.end()) {
						val.erase(ifind);
					}
				}
				if (val.empty()) {
					_removes.push(key);
				}
			});
		if (!_removes.empty()) {
			auto node = _removes.pop();
			if (node) {
				m_TimerQ.pop(*node, [](const auto&, auto&) {});
				m_TimestampBakQ.pop(*node, [&](const auto&, auto&) {});
			}
		}
	}



}///namespace shared

/// /*新生®（上海）**/
/// /*2022_01_05T02:49:45.9010235Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___9B4F0A2C_6D72_4637_BAEA_91E3139C6F3A__HEAD__