#ifndef Timers_h
#define Timers_h


#include "Events.h"
#include "TimeSlice.h"
#include "DueTimer.h"
#include "advancedFunctions.h"
#include "Logger.h"

struct TimerEvent
{
	Events* handler;
	int te_id;
	uint32_t lastfire;
	uint32_t resolution;
	bool onlyOnce;
};

class CTimers : public Events
{
public:
	CTimers(void);
	~CTimers(void);
	void Begin(void);
	bool Started(void);
	void OneSecondElapsed();

	void AddOneSecHandler(Events* handler, int event_id);
	void AddTimerHandler(Events* handler, int event_id, uint32_t resolution, bool onlyOnce = false);
	void AddMinuteStartHandler(Events* handler, int event_id);
	void ChangeResolution(Events* handler, uint32_t new_resolution);

private:
	void OnTimeSlice();
	bool started;
	volatile bool oneSecFired;
	TimerEvent* tevent;
	StaticList<16, TimerEvent> timerHandlers;
	StaticList<16, TimerEvent> oneSecHandlers;
	StaticList<16, TimerEvent> minuteStartHandlers;
	bool minStartFired;
	uint8_t last_min;

	void DispatchMinuteStartEvent(uint32_t m);
	void DispatchOneSecEvent(uint32_t m);
	void DispatchTimerResolutionEvent(uint32_t m);
};

extern CTimers Timers;

#endif // !Timers_h
