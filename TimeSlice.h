#ifndef TimeSlice_h
#define TimeSlice_h

#include "StaticList.h"
#include "Events.h"

class CTimeSlice
{
public:
	CTimeSlice(void);
	~CTimeSlice(void);
	void RegisterTimeSlice(Events* value);
	void UnRegisterTimeSlice(Events* value);
	void TimeSlice();
private:
	StaticList<64, Events*> slices;
	void DispatchTimeSlice();
};


extern CTimeSlice TimeSlice;

#endif