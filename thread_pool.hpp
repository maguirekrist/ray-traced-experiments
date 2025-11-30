#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

class ThreadPool {
	using Job = std::function<void()>;
public:

	explicit ThreadPool(size_t n = std::max(1u, std::thread::hardware_concurrency())) {
		for(size_t i = 0; i < n; i++)
		{
			_threads.emplace_back([this](std::stop_token st) {
				for(;;)
				{
					Job job;
					{
						std::unique_lock lk(_mutex);
						_cv.wait(lk, [this, st]{ return stop || !_queue.empty() || st.stop_requested(); });
						if (stop && _queue.empty()) break;
						if (st.stop_requested()) break;

						job = std::move(_queue.front());
						_queue.pop();
					}

					//Job begin
					job();
					{
						std::lock_guard lk(_mutex);
						if (stop && _queue.empty()) {
							_cv.notify_all();
							break;
						}
					}
				}	
			});
		
		}
	}

	~ThreadPool() {
		{
			std::lock_guard lk(_mutex);
			stop = true;
		}

		_cv.notify_all();

		{
			std::unique_lock lk(_mutex);
			_cv.wait(lk, [this] {
				return _queue.empty();
			});
		}

	}

	void execute(Job&& new_job) {
		std::unique_lock lk(_mutex);
		if (stop) {
			throw std::runtime_error("ThreadPool stopped!");
		}
		_queue.push(std::move(new_job));
		_cv.notify_one();
	};

private:
	std::mutex _mutex;
	std::condition_variable _cv;
	std::queue<Job> _queue;
	bool stop = false;
	std::vector<std::jthread> _threads;
};
