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
	STATUS_EXHAUSTFAN_MANUAL
};

const char* const cUNKNOWN = "UNKNOWN";
const char* const cOFF = "OFF";
const char* const cON = "ON";
const char* const cMANUAL = "MANUAL";
const char* const cDISABLED = "DISABLED";
const char* const cDISABLED_OFF = "DISABLED_OFF";
const char* const cDISABLED_ON = "DISABLED_ON";
const char* const cKATILAS_UNKNOWN = "UNKNOWN";
const char* const cKATILAS_UZGESES = "UZGESES";
const char* const cKATILAS_IKURIAMAS = "IKURIAMAS";
const char* const cKATILAS_KURENASI = "KURENASI";
const char* const cKATILAS_GESTA = "GESTA";
const char* const cLADOMAT_OFF = "OFF";
const char* const cLADOMAT_ON = "ON";
const char* const cLADOMAT_MANUAL = "MANUAL";
const char* const cEXHAUSTFAN_OFF = "OFF";
const char* const cEXHAUSTFAN_ON = "ON";
const char* const cEXHAUSTFAN_MANUAL = "MANUAL";
const char* const cSHIT = "SHIT";

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
	virtual void OnTimeSlice(void);

	virtual void Reset(void);

	static const char* GetStatusName(Status value);
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
