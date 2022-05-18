#ifndef List_h
#define List_h

#include <arduino.h>

template<typename T>
class List
{
public:
	List(void);
	~List(void);
	uint8_t Count();
	bool Add(T* item);
	T* Get(uint8_t idx);
	void Delete(uint8_t idx);
	void Delete(T* item);
	void Clear();
private:
	uint8_t count;
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
uint8_t List<T>::Count()
{
	return count;
}

template<typename T>
bool List<T>::Add(T* item)
{
	//Serial.println("DBG: Creating node...");
	node* n = new node;
	if(n == NULL){ 
		Serial.println("0 DBG: List Node NOT created.");
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
	else
	{
		first = n;
		last = n;
	}
	count++;
	return true;
}

template<typename T>
T* List<T>::Get(uint8_t idx)
{
	if(idx>=0 && idx < count)
	{
		node *tmp = first;
		uint8_t i = 0;
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
void List<T>::Delete(uint8_t idx)
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
			delete tmp;
			count--;
		}
		else//deleting first element
		{
			tmp = first->next;
			if(tmp == NULL) last = NULL;
			delete first;
			first = tmp;
			count--;
		}
	}
}

template<typename T>
void List<T>::Delete(T* item)
{
	if (item == NULL) return;
	if(count <= 0 ) return;

	node *tmp, *prev;
	tmp = first;
	prev = NULL;

	for (uint8_t i = 0; i < count; i++)
	{
		if (tmp != NULL)
		{
			if (tmp->item == item)
			{
				if (prev)
				{
					prev->next = tmp->next;
					if (prev->next == NULL) last = prev;
					delete tmp;
				}
				else//deleting first element
				{
					tmp = first->next;
					if (tmp == NULL) last = NULL;
					delete first;
					first = tmp;
				}
				count--;
				break;
			}
			else
			{
				prev = tmp;
				tmp = tmp->next;
			}
		}
		else
		{
			break;
		}
	}
}

template<typename T>
void List<T>::Clear()
{
	node* p = first;
    while ( p != NULL )
    {
    	node* nextNode = p->next;
    	delete p;
    	p = nextNode;
    }
	count = 0;
	first = NULL;
	last = NULL;
}

#endif