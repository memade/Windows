#if !defined(AFX_SK_H__B18DBD34_0DDC_4860_867E_3B6D287F5277__HEAD__)
#define AFX_SK_H__B18DBD34_0DDC_4860_867E_3B6D287F5277__HEAD__

namespace shared {
	namespace thread {
#if __ExampleCode
		{//! return value
			std::vector< std::future<int> > results;
			for (int i = 0; i < 8; ++i) {
				results.emplace_back(
					pool.enqueue([i] {
						std::cout << "hello " << i << std::endl;
						std::this_thread::sleep_for(std::chrono::seconds(1));
						std::cout << "world " << i << std::endl;
						return i * i;
						})
				);
			}
			for (auto&& result : results) {
				std::cout << result.get() << ' ';
			}
			std::cout << std::endl;
		}///! return value
		{//! return void
			for (int i = 0; i < 10; i++) {
				pThreadPool->enqueue([i] {
					std::cout << "abc123" << " " << i << std::endl;
					});
			}///for (int i = 0; i < 10; i++)
		}///! return void
#endif///ExampleCode
		class ThreadPool final
		{
		public:
			ThreadPool(size_t threads = 4) : stop(false) {
					for (size_t i = 0; i < threads; ++i)
						workers.emplace_back(
							[this]
							{
								for (;;)
								{
									std::function<void()> task;
									{
										std::unique_lock<std::mutex> lock(this->queue_mutex);
										this->condition.wait(lock,
											[this] { return this->stop || !this->tasks.empty(); });
										if (this->stop && this->tasks.empty())
											return;
										task = std::move(this->tasks.front());
										this->tasks.pop();
									}
									task();
								}
							});
			}
			~ThreadPool() {
				{
					std::unique_lock<std::mutex> lock(queue_mutex);
					stop = true;
				}
				condition.notify_all();
				for (std::thread& worker : workers)
					worker.join();
			}
#if 0
			template<class F, class... Args>
			auto enqueue(F&& f, Args&& ... args)->std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>;
#else
			template<class F, class... Args>
			auto enqueue(F&& f, Args&& ... args)->std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
			{
				auto task = std::make_shared<std::packaged_task<decltype(std::forward<F>(f)(std::forward<Args>(args)...))()>>(
					std::bind(std::forward<F>(f), std::forward<Args>(args)...)
					);
				std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))> res = task->get_future();
				{
					std::unique_lock<std::mutex> lock(queue_mutex);
					// don't allow enqueueing after stopping the pool
					if (stop)
						throw std::runtime_error("enqueue on stopped ThreadPool");
					tasks.emplace([task]() { (*task)(); });
				}
				condition.notify_one();
				return res;
			}
#endif
		private:
			// need to keep track of threads so we can join them
			std::vector<std::thread > workers;
			// the task queue
			std::queue<std::function<void()>> tasks;
			// synchronization
			std::mutex queue_mutex;
			std::condition_variable condition;
			bool stop;
		};
	}///namespace thread
}///namespace shared


/*!@ 新生联创®（上海）*/
/*!@ Mon Aug 2 22:20:51 UTC+0800 2021*/
/*!@ ___www.skstu.com___*/
#endif /*AFX_SK_H__B18DBD34_0DDC_4860_867E_3B6D287F5277__HEAD__*/