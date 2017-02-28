#include "task_pool.h"

static TaskPool* g_instance = nullptr;

TaskPool* TaskPool::GetInstance() {
	if (g_instance == nullptr) {
		g_instance = new TaskPool();
	}
	return g_instance;
}

void TaskPool::Start(int32_t threadNum) {
	for (int32_t i = 0; i < threadNum; i++) {
		m_workThreads.push_back(std::thread([this]() {
			while (true) {
				std::unique_lock<std::mutex> queueLock(this->m_queueMutex);
				this->m_conditionVar.wait(queueLock, [this]() {
					return !this->m_taskQueue.empty() || !this->m_isStop;
				});
				if (this->m_isStop && this->m_taskQueue.empty()) {
					return;
				}
				std::function<void()> task = std::move(this->m_taskQueue.front());
				this->m_taskQueue.pop();
				task();
			}
		}));
	}
}

template<class F, class... Args> auto TaskPool::AddTask(F&& f, Args&&... args)
	->std::future<typename std::result_of<F(Args...)>::type> {
	using return_type = typename std::result_of<F(Args...)>::type;
	auto task = std::make_shared<std::packaged_task<return_type()>> (
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
	);

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> queueLock(m_queueMutex);
		if (m_isStop) {
			return;
		}
		m_taskQueue.emplace([task](){ (*task)(); });
	}
	m_conditionVar.notify_one();
	return res;
}

void TaskPool::Stop() {
	std::unique_lock<std::mutex> queueLock(this->m_queueMutex);
	m_isStop = true;
	m_conditionVar.notify_all();
	for (auto &thread : m_workThreads) {
		thread.join();
	}
}


