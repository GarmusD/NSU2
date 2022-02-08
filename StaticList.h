#ifndef StaticList_h
#define StaticList_h

#include <stdlib.h>
#include "Logger.h"

template<size_t C, typename T>
class StaticList
{
public:
	StaticList(void);
	~StaticList(void);
	int Count();
	int Capacity();
	T* Add(void);
	T* Get(size_t idx);
	void Set(int idx, T item);
	int FindFree();
	void Delete(int idx);
	void Delete(T item);
	void Delete(T* item);
	void Clear();
	T* operator[](size_t idx);
private:
	struct node {
      T item;      // the item in the node.
      bool isNull; // NULL indicator
    };
	node elements [C];	
};

template<size_t C, typename T>
StaticList<C, T>::StaticList(void)
{
	for (int i = 0; i < C; i++)
	{
		elements[i].isNull = true;
	}
}

template<size_t C, typename T>
StaticList<C, T>::~StaticList(void)
{
	Clear();
}

template<size_t C, typename T>
int StaticList<C, T>::Count()
{
	int res = 0;
	for (int i = 0; i < C; i++)
	{
		if (!elements[i].isNull)
		{
			res++;
		}
	}
	return res;
}

template<size_t C, typename T>
int StaticList<C, T>::Capacity()
{
	return C;
}

template<size_t C, typename T>
T* StaticList<C, T>::Add(void)
{
	for (int i = 0; i < C; i++)
	{
		if (elements[i].isNull == true)
		{
			elements[i].isNull = false;
			return &elements[i].item;
		}
	}
	return NULL;
}

template<size_t C, typename T>
T* StaticList<C, T>::Get(size_t idx)
{
	if(idx>=0 && idx < C)
	{
		if (elements[idx].isNull)
			return NULL;
		else
			return &elements[idx].item;
	}
	return NULL;
}

template<size_t C, typename T>
void StaticList<C, T>::Set(int idx, T item)
{
	if (idx >= 0 && idx < C)
	{
		elements[idx].isNull = false;
		elements[idx].item = item;
	}
}

template<size_t C, typename T>
int StaticList<C, T>::FindFree(void)
{
	for (int i = 0; i < C; i++)
	{
		if (elements[i].isNull)
		{
			elements[i].isNull = false;
			return i;
		}
	}
	return -1;
}

template<size_t C, typename T>
void StaticList<C, T>::Delete(int idx)
{
	if(idx>=0 && idx < C)
	{
		elements[idx].isNull = true;
	}
}

template<size_t C, typename T>
void StaticList<C, T>::Delete(T item)
{
	for (int i = 0; i < C; i++)
	{
		T var = elements[i].item;
		if (var == item)
		{
			elements[i].isNull = true;
		}
	}
}

template<size_t C, typename T>
void StaticList<C, T>::Delete(T* item)
{
	for (int i = 0; i < C; i++)
	{
		T* var = &elements[i].item;
		if (var == item)
		{
			elements[i].isNull = true;
		}
	}
}

template<size_t C, typename T>
void StaticList<C, T>::Clear()
{
	for (int i = 0; i < C; i++)
	{
		elements[i].item.CleanUp();
		delete elements[i].item;
		elements[i].item = new T();
		elements[i].isNull = true;
	}
}

template<size_t C, typename T>
T* StaticList<C, T>::operator[](size_t idx)
{
	if (idx >= 0 && idx < C)
	{
		if (elements[idx].isNull)
			return NULL;
		else
			return &elements[idx].item;
	}
	return NULL;
}

#endif