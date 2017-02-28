#include "object_pool.h"

template<typename T>
ObjectPool<T>::ObjectPool(int objectNum) {
	for (int i = 0; i < objectNum; i++) {
		m_objects.push_back(new T());
	}
}

template<typename T>
T& ObjectPool<T>::AcquireObject() {
	if (m_objects.empty() == false) {
		T* obj = m_objects.front();
		m_objects.pop_front();
		return (*obj);
	}
	return nullptr;
}

template<typename T>
void ObjectPool<T>::ReleaseObject(T& obj) {
	m_objects.push_back(&obj);
}

