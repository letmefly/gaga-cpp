#ifndef __OBJECT_POOL_H__
#define __OBJECT_POOL_H__
#include <list>

template<typename T>
class ObjectPool
{
public:
	ObjectPool(int objectNum);
	~ObjectPool() {};

	T& AcquireObject();
	void ReleaseObject(T& obj);

private:
	std::list<T*> m_objects;
};


#endif

