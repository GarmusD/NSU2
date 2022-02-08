#include "Timers.h"


void OneSecondInt()
{
	Timers.OneSecondElapsed();
}

void CTimers::OnTimeSlice()
{
	if (oneSecFired)
	{
		oneSecFired = false;
		uint32_t m = millis();
		uint8_t min = rtc.getMinutes();

		DispatchOneSecEvent(m);

		DispatchTimerResolutionEvent(m);

		if (min != last_min)
		{
			last_min = min;
			DispatchMinuteStartEvent(m);
		}
	}
}

CTimers::CTimers(void)
{
	started = false;
}


CTimers::~CTimers(void)
{
	timerHandlers.Clear();
	oneSecHandlers.Clear();
	minuteStartHandlers.Clear();
}

void CTimers::Begin()
{
	uint32_t m = millis();
	for(int i=0; i < timerHandlers.Count(); i++)
	{
		timerHandlers.Get(i)->lastfire = m;
	}
	for(int i=0; i < oneSecHandlers.Count(); i++)
	{
		oneSecHandlers.Get(i)->lastfire = m;
	}
	for(int i=0; i < minuteStartHandlers.Count(); i++)
	{
		minuteStartHandlers.Get(i)->lastfire = m;
	}
	last_min = rtc.getMinutes();
	started = true;
	Timer.getAvailable().attachInterrupt(OneSecondInt).setFrequency(1).start();
	TimeSlice.RegisterTimeSlice(this);
}

bool CTimers::Started(void)
{
	return started;
}

void CTimers::AddOneSecHandler(Events* handler, int event_id)
{
	tevent = oneSecHandlers.Add();
	if (tevent)
	{
		tevent->handler = handler;
		tevent->te_id = event_id;
	}
}

void CTimers::AddTimerHandler(Events* handler, int event_id, uint32_t resolution, bool onlyOnce/* = false*/)
{
	tevent = timerHandlers.Add();
	if (tevent)
	{
		tevent->handler = handler;
		tevent->te_id = event_id;
		tevent->lastfire = millis();
		tevent->resolution = resolution;
		tevent->onlyOnce = onlyOnce;
	}
}

void CTimers::AddMinuteStartHandler(Events* handler, int event_id)
{
	tevent = minuteStartHandlers.Add();
	if (tevent)
	{
		tevent->handler = handler;
		tevent->te_id = event_id;
		tevent->lastfire = millis();
		tevent->resolution = 0;
	}
}

void CTimers::ChangeResolution(Events* handler, uint32_t new_resolution)
{
	for(int i = 0; i < timerHandlers.Count(); i++)
	{
		tevent = timerHandlers.Get(i);
		if(tevent->handler == handler && tevent->resolution != new_resolution)
		{
			tevent->resolution = new_resolution;
			tevent->lastfire = millis();
		}
	}
}

void CTimers::OneSecondElapsed()
{
	oneSecFired = true;
}

void CTimers::DispatchOneSecEvent(uint32_t m)
{
	for(int i=0; i < oneSecHandlers.Count(); i++)
	{
		tevent = oneSecHandlers.Get(i);
		tevent->handler->HandleTimerEvent(tevent->te_id);
	}
}

void CTimers::DispatchMinuteStartEvent(uint32_t m)
{
	for(int i=0; i < minuteStartHandlers.Count(); i++)
	{
		tevent = minuteStartHandlers.Get(i);
		tevent->handler->HandleTimerEvent(tevent->te_id);
	}
}

void CTimers::DispatchTimerResolutionEvent(uint32_t m)
{
	bool done = false;
	while(!done)
	{
		for(int i=0; i < timerHandlers.Count(); i++)
		{
			tevent = timerHandlers.Get(i);
			if( (m-tevent->lastfire) >= tevent->resolution)
			{
				tevent->lastfire = m;
				tevent->handler->HandleTimerEvent(tevent->te_id);
				if(tevent->onlyOnce)
				{
					timerHandlers.Delete(tevent);
					break;
				}
			}
		}
		done = true;
	}
}


CTimers Timers;