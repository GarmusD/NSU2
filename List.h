#ifndef List_h
#define List_h

#include <stdlib.h>

template<typename T>
class List
{
public:
	List(void);
	~List(void);
	int Count();
	bool Add(T* item);
	T* Get(int idx);
	void Delete(int idx);
	void Delete(T* item);
	void Clear();
private:
	int count;
	struct node {
      T* item;      // the item in the node.
      node * next; // the next node in the list.
    };
	node *first, *last;
};

template<typename T>
List<T>::List(void)
{
	count = 0;
	first = NULL;
	last = NULL;
}

template<typename T>
List<T>::~List(void)
{
	Clear();
	count = 0;
	first = NULL;
	last = NULL;
}

template<typename T>
int List<T>::Count()
{
	return count;
}

template<typename T>
bool List<T>::Add(T* item)
{
	//Serial.println("DBG: Creating node...");
	node* n = new node;
	if(n == NULL){ 
		//Serial.println("DBG: Node NOT created.");
		return false; 
	}
	//Serial.println("DBG: Node created.");
	n->item = item;
	n->next = NULL;
	if(last)
	{
		last->next = n;
		last = n;
	}
	if(!first)
	{
		first = n;
		last = n;
	}
	count++;
	return true;
}

template<typename T>
T* List<T>::Get(int idx)
{
	if(idx>=0 && idx < count)
	{
		node *tmp;
		tmp = first;
		int i = 0;
		while(i != idx)
		{
			tmp = tmp->next;
			i++;
		}
		return tmp->item;;
	}
	return NULL;
}

template<typename T>
void List<T>::Delete(int idx)
{
	if(idx>=0 && idx < count)
	{
		node *tmp, *prev;
		tmp = first;
		prev = NULL;
		int i = 0;
		while(i != idx)
		{
			prev = tmp;
			tmp = tmp->next;
			i++;
		}
		if(prev)
		{
			prev->next = tmp->next;
			if(prev->next == NULL) last = prev;
			//if(tmp->item) delete tmp->item;
			delete tmp;
			count--;
		}
		else//deleting first element
		{
			tmp = first->next;
			if(tmp == NULL) last = NULL;
			//if(first->item) delete first->item;
			delete first;
			first = tmp;
			count--;
		}
	}
}

template<typename T>
void List<T>::Delete(T* item)
{
	if(count <= 0 ) return;

	node *tmp, *prev;
	tmp = first;
	prev = NULL;

	while(tmp->item != item || tmp != NULL)
	{
		prev = tmp;
		tmp = tmp->next;
	}
	if(prev)
	{
		prev->next = tmp->next;
		if(prev->next == NULL) last = prev;
		delete tmp;
		count--;
	}
	else//deleting first element
	{
		tmp = first->next;
		if(tmp == NULL) last = NULL;
		delete first;
		first = NULL;
		count--;
	}
}

template<typename T>
void List<T>::Clear()
{
	node* p = first;
    while ( p!=NULL )
    {
    	node* nextNode = p->next;
    	delete p;
    	p = nextNode;
    }
}

#endif