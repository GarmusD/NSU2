#ifndef Events_h
#define Events_h

#include "List.h"
#include <Arduino.h>

class Events;

enum Status
{
	STATUS_UNKNOWN,
	STATUS_OFF,
	STATUS_ON,
	STATUS_MANUAL,
	STATUS_DISABLED,
	STATUS_DISABLED_OFF,
	STATUS_DISABLED_ON,
	STATUS_KATILAS_UNKNOWN,
	STATUS_KATILAS_UZGESES,
	STATUS_KATILAS_IKURIAMAS,
	STATUS_KATILAS_KURENASI,
	STATUS_KATILAS_GESTA,
	STATUS_LADOMAT_OFF,
	STATUS_LADOMAT_ON,
	STATUS_LADOMAT_MANUAL,
	STATUS_EXHAUSTFAN_OFF,
	STATUS_EXHAUSTFAN_ON,
	STATUS_EXHAUST_MANUAL
};

enum Tags
{
	TAG_NONE,
	TAG_CLICK
};


class Events
{
public:
	Events(void);
	virtual ~Events(void);

	void AddTempChangeHandler(Events* handler);
	void AddOnClickHandler(Events* handler);
	void AddStatusChangeHandler(Events* handler);

	void RemoveTempChangeHandler(Events* handler);
	void RemoveOnClickHandler(Events* handler);
	void RemoveStatusChangeHandler(Events* handler);	

	virtual void HandleTimerEvent(int te_id);
	virtual void OnTimeSlice();

	virtual void CleanUp();

	static String GetStatusName(Status value);
protected:
	void DispatchTemperature(void* Sender, float t);
	void DispatchOnClick(void* Sender, int x, int y, int tag);
	void DispatchStatusChange(void* Sender, Status status);

	virtual void HandleTemperatureChange(void* Sender, float t);
	virtual void HandleOnClick(void* Sender, int x, int y, int tag);
	virtual void HandleStatusChange(void* Sender, Status status);
private:
	List<Events> TempHandlers;
	List<Events> OnClickHandlers;
	List<Events> StatusHandlers;
};

#endif
