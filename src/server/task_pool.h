#ifndef __TASK_POOL_H__
#define __TASK_POOL_H__

#include <vector>
#include <queue>
#include <string>
#include <thread>
#include <future>
#include <mutex>
#include <functional>
#include <stdexcept>
#include <condition_variable>

class TaskPool {
public:
	TaskPool* GetInstance();
	void Start(int32_t threadNum);
	void Stop();
	template<class F, class... Args> auto AddTask(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>;

private:
	TaskPool() :m_isStop(false) {};
	~TaskPool() {};

	bool m_isStop;
	std::vector<std::thread> m_workThreads;
	std::mutex m_queueMutex;
	std::condition_variable m_conditionVar;
	std::queue<std::function<void()>> m_taskQueue;
};

#endif
