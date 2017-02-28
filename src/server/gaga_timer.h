#ifndef __GAGA_TIMER_H__
#define __GAGA_TIMER_H__

#include <inttypes.h>
#include <functional>
#include <list>
#include <vector>
#include <map>

struct TimerNode {
	std::string name;
	uint64_t expireTime;
	std::function<void()> cb;
};

struct TimerSlot {
	std::list<TimerNode*> timerList;
};

class TimerManager {
public:
	static TimerManager* GetInstance();
	int32_t AddTimeoutTimer(const std::string name, const uint32_t timeoutTicks, const std::function<void()>& cb);
	int32_t AddTickTimer(const std::string name, const uint32_t ticks, const std::function<void()>& tick_cb);
	void RemoveTimer(const std::string name);
	void Tick();

private:
	TimerManager();
	~TimerManager();
	void InitTimerSlots(std::vector<TimerSlot*>& slotVector, uint32_t num);
	TimerNode* FindTimerNode(const std::string name);
	uint64_t GetOSTime();
	void GetTimeInfo(uint64_t milSecond, uint32_t& hour, uint32_t& minute, uint32_t& second, uint32_t& tick);
	uint32_t m_maxHour;
	uint32_t m_maxMinute;
	uint32_t m_maxSecond;
	uint32_t m_maxTick;
	std::list<TimerNode*> m_tickTimers;
	std::vector<TimerSlot*> m_hourSlots;
	std::vector<TimerSlot*> m_minuteSlots;
	std::vector<TimerSlot*> m_secondSlots;
	std::vector<TimerSlot*> m_tickSlots;
	std::map<std::string, TimerNode*> m_timerNodeMap;
	uint64_t m_totalTicks;
	uint32_t m_lastTickSeq;
};

#endif
