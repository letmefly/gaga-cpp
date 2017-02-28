#ifndef __OBJECT_POOL_H__
#define __OBJECT_POOL_H__
#include <list>
#include <functional>

template<typename T>
class ObjectPool
{
public:
	ObjectPool(int num) {
		for (int i = 0; i < num; i++) {
			m_objects.push_back(new T);
		}
	}
	~ObjectPool() {}

	T* AcquireObject() {
		if (false == m_objects.empty()) {
			T* obj = m_objects.front();
			m_objects.pop_front();
			return obj;
		}
		return nullptr;
	}
	void ReleaseObject(T* obj) {
		m_objects.push_back(obj);
	}
private:
	std::list<T*> m_objects;
};


#endif

