#ifndef TimeSlice_h
#define TimeSlice_h

#include "StaticListP.h"
#include "Events.h"

class CTimeSlice
{
public:
	CTimeSlice(void);
	~CTimeSlice(void);
	void RegisterTimeSlice(Events* value);
	void UnRegisterTimeSlice(Events* value);
	void TimeSlice();
	void Reset();
private:
	StaticListP<64, Events*> slices;
	void DispatchTimeSlice();
};


extern CTimeSlice TimeSlice;

#endif