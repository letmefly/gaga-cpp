#include "gaga_timer.h"
#include "gaga_util.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#define MAX_HOUR_BIT 8
#define MAX_MINUTE_BIT 6
#define MAX_SECOND_BIT 6
#define MAX_TICK_BIT 6
#define HOUR_MASK ((1 << MAX_HOUR_BIT) - 1)
#define MINUTE_MASK ((1 << MAX_MINUTE_BIT) - 1)
#define SECOND_MASK ((1 << MAX_SECOND_BIT) - 1)
#define TICK_MASK ((1 << MAX_TICK_BIT) - 1)

static TimerManager* g_instance = nullptr;
TimerManager* TimerManager::GetInstance() {
	if (g_instance == nullptr) {
		g_instance = new TimerManager();
	}
	return g_instance;
}

TimerManager::TimerManager () {
	m_totalTicks = 0;
	m_lastTickSeq = 0xFFFFFFFF;
	m_maxHour = 1 << MAX_HOUR_BIT;
	m_maxMinute = 1 << MAX_MINUTE_BIT;
	m_maxSecond = 1 << MAX_SECOND_BIT;
	m_maxTick = 1 << MAX_TICK_BIT;

	InitTimerSlots(m_hourSlots, m_maxHour);
	InitTimerSlots(m_minuteSlots, m_maxMinute);
	InitTimerSlots(m_secondSlots, m_maxSecond);
	InitTimerSlots(m_tickSlots, m_maxTick);
}

void TimerManager::InitTimerSlots(std::vector<TimerSlot*>& slotVector, uint32_t num) {
	for (uint32_t i = 0; i < num; i++) {
		slotVector.push_back(new TimerSlot());
	}
}

int32_t TimerManager::AddTimeoutTimer(const std::string name, const uint32_t timeoutTicks, const std::function<void()>& cb) {
	uint64_t osTime = GetOSTime();
	uint64_t expireTime = (osTime >> 4) + timeoutTicks;
	if (FindTimerNode(name)) {
		gaga_printf("[TimerManager]ERR:%s is exist, you should remove first\n", name.c_str());
		return -1;
	}
	TimerNode *timerNode = new TimerNode();
	m_timerNodeMap.insert(std::pair<std::string, TimerNode*>(name, timerNode));
	timerNode->expireTime = expireTime;
	timerNode->cb = cb;
	timerNode->name = name;

	uint32_t hour, minute, second, tick;
	GetTimeInfo(expireTime << 4, hour, minute, second, tick);

	if (hour == 0) {
		if (minute == 0) {
			if (second == 0) {
				auto tickSlot = m_tickSlots[tick];
				if (tickSlot != nullptr) {
					tickSlot->timerList.push_back(timerNode);
				}
			}
			else {
				auto secondSlot = m_secondSlots[second];
				if (secondSlot != nullptr) {
					secondSlot->timerList.push_back(timerNode);
				}
			}
		}
		else {
			auto minuteSlot = m_minuteSlots[minute];
			if (minuteSlot != nullptr) {
				minuteSlot->timerList.push_back(timerNode);
			}
		}
	}
	else {
		auto hourSlot = m_hourSlots[hour];
		if (hourSlot != nullptr) {
			hourSlot->timerList.push_back(timerNode);
		}
	}

	return 0;
}

int32_t TimerManager::AddTickTimer(const std::string name, const uint32_t ticks, const std::function<void()>& tick_cb) {
	if (FindTimerNode(name)) {
		gaga_printf("[TimerManager]ERR:%s is exist, you should remove first\n", name.c_str());
		return -1;
	}
	TimerNode *timerNode = new TimerNode();
	m_timerNodeMap.insert(std::pair<std::string, TimerNode*>(name, timerNode));
	timerNode->expireTime = ticks;
	timerNode->cb = tick_cb;
	timerNode->name = name;
	m_tickTimers.push_back(timerNode);
	return 0;
}

TimerNode* TimerManager::FindTimerNode(const std::string name) {
	auto it = m_timerNodeMap.find(name);
	if (it != m_timerNodeMap.end()) {
		return it->second;
	}
	return nullptr;
}

void TimerManager::RemoveTimer(const std::string name)
{
	auto it = m_timerNodeMap.find(name);
	if (it != m_timerNodeMap.end()) {
		auto timerNode = it->second;
		timerNode->expireTime = 0;
	}
	m_timerNodeMap.erase(name);
}

uint64_t TimerManager::GetOSTime() {
	uint64_t t;
#ifdef _WIN32
	SYSTEMTIME st;
	GetSystemTime(&st);
	t = ((st.wHour * 60 + st.wMinute) * 60 + st.wSecond) * 1000 + st.wMilliseconds;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	t = (uint64_t)tv.tv_sec * 100;
	t += tv.tv_usec / 10000;
#endif
	return t;
}

void TimerManager::GetTimeInfo(uint64_t milSecond, uint32_t& hour, uint32_t& minute, uint32_t& second, uint32_t& tick) {
	// 16ms = 1 tick
	uint64_t currTick = milSecond >> 4;
	hour = (currTick >> (MAX_MINUTE_BIT + MAX_SECOND_BIT + MAX_TICK_BIT)) & HOUR_MASK;
	minute = (currTick >> (MAX_SECOND_BIT + MAX_TICK_BIT)) & MINUTE_MASK;
	second = (currTick >> (MAX_TICK_BIT)) & SECOND_MASK;
	tick = currTick & TICK_MASK;
}

void TimerManager::Tick() {
	uint32_t tmpHour, tmpMinute, tmpSecond, tmpTick;
	uint64_t osTime = GetOSTime();
	uint32_t currHour, currMinute, currSecond, currTick;
	GetTimeInfo(osTime, currHour, currMinute, currSecond, currTick);

	// 1. processing tick timer
	if (m_lastTickSeq != currTick) {
		++m_totalTicks;
		m_lastTickSeq = currTick;
		TimerNode *removeNode = nullptr;
		for (TimerNode *node : m_tickTimers) {
			if (node->expireTime != 0) {
				if (m_totalTicks % node->expireTime == 0) {
					node->cb();
				}
			}
			else {
				removeNode = node;
			}
		}
		if (removeNode) {
			m_tickTimers.remove(removeNode);
		}
	}

	// 2. processing timeout timer
	auto hourSlot = m_hourSlots[currHour];
	auto minuteSlot = m_minuteSlots[currMinute];
	auto secondSlot = m_secondSlots[currSecond];
	auto tickSlot = m_tickSlots[currTick];
	if (hourSlot->timerList.empty() == true) {
		if (minuteSlot->timerList.empty() == true) {
			if (secondSlot->timerList.empty() == true) {
				if (tickSlot->timerList.empty() == true) {
					return;
				}
				else {
					// now time is up!
					while (tickSlot->timerList.empty() == false) {
						auto timerNode = tickSlot->timerList.front();
						auto cb = timerNode->cb;
						auto name = timerNode->name;
						auto expireTime = timerNode->expireTime;
						if (expireTime != 0) {
							cb();
						}
						tickSlot->timerList.pop_front();
						m_timerNodeMap.erase(name);
						delete timerNode;
					}
				}
			}
			else {
				// 1. execute all unticked timer, if tick number is not continue within 1 s
				for (uint32_t i = 0; i < m_maxTick; i++) {
					auto tmpTickSlot = m_tickSlots[i];
					while (tmpTickSlot->timerList.empty() == false) {
						auto timerNode = tmpTickSlot->timerList.front();
						auto cb = timerNode->cb;
						auto name = timerNode->name;
						auto expireTime = timerNode->expireTime;
						if (expireTime != 0) {
							cb();
						}
						tmpTickSlot->timerList.pop_front();
						m_timerNodeMap.erase(name);
						delete timerNode;
					}
				}
				
				// 2. now put all timer nodes into tickSlots
				while (secondSlot->timerList.empty() == false) {
					auto timerNode = secondSlot->timerList.front();
					GetTimeInfo((timerNode->expireTime) << 4, tmpHour, tmpMinute, tmpSecond, tmpTick);
					auto insertTickSlot = m_tickSlots[tmpTick];
					insertTickSlot->timerList.push_back(timerNode);
					secondSlot->timerList.pop_front();
				}
				
			}
		}
		else {
			// now put all timer nodes into secondSlots
			while (minuteSlot->timerList.empty() == false) {
				auto timerNode = minuteSlot->timerList.front();
				GetTimeInfo((timerNode->expireTime) << 4, tmpHour, tmpMinute, tmpSecond, tmpTick);
				auto secondSlot = m_secondSlots[tmpSecond];
				secondSlot->timerList.push_back(timerNode);
				minuteSlot->timerList.pop_front();
			}
		}
	}
	else {
		// now put all timer nodes into minuteSlots
		while (hourSlot->timerList.empty() == false) {
			auto timerNode = hourSlot->timerList.front();
			GetTimeInfo((timerNode->expireTime) << 4, tmpHour, tmpMinute, tmpSecond, tmpTick);
			auto minuteSlot = m_minuteSlots[tmpMinute];
			minuteSlot->timerList.push_back(timerNode);
			hourSlot->timerList.pop_front();
		}
	}
}



