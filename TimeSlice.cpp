#include "TimeSlice.h"


CTimeSlice::CTimeSlice(void)
{
}


CTimeSlice::~CTimeSlice(void)
{
}

void CTimeSlice::RegisterTimeSlice(Events* value)
{
	int idx = slices.FindFree();
	if (idx >= 0)
	{
		slices.Set(idx, value);
	}
}

void CTimeSlice::UnRegisterTimeSlice(Events* value)
{
	slices.Delete(value);
}

void CTimeSlice::TimeSlice()
{
	DispatchTimeSlice();
}

void CTimeSlice::DispatchTimeSlice()
{
	for(int i = 0; i < slices.Count(); i++)
	{
		Events** ppe = slices.Get(i);
		Events* pe = *ppe;
		pe->OnTimeSlice();
	}
}



CTimeSlice TimeSlice;