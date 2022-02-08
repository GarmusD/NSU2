#ifndef Collector_h
#define Collector_h

#include "StaticList.h"
#include "Events.h"
#include "CirculationPump.h"
#include "RelayModule.h"
#include "ThermoValve.h"
#include "JsonPrinter.h"

#define MAX_COLLECTORS 8
#define MAX_COLLECTOR_VALVES 16

class Collector:public Events
{
public:
	Collector(void);
	~Collector(void);
	void OpenValve(byte valve);
	void CloseValve(byte valve);
	void SetValveCount(byte value);
	void SetThermoValve(byte idx, ThermoValve valve);
	void SetCirculationPump(CirculationPump* circpump);
	void SetName(const char* name);
	const char* GetName();
	String GetInfoString();
	byte GetValveCount();
	bool GetValveStatus(byte pos);
	void PrintStatus();
private:
	char name[MAX_NAME_LENGTH];
	int count;
	bool pump_enabled, channels_closed;
	CirculationPump* pump;
	//RelayModule* rm;
	ThermoValve valves[MAX_COLLECTOR_VALVES];
	bool valve_status[MAX_COLLECTOR_VALVES];
	void HandleStatusChange(void* Sender, Status status);
	void CloseRelayChannels();
	void OpenRelayChannels();
};

class CCollectors: public StaticList<MAX_COLLECTORS, Collector>
{
public:
	Collector* GetByName(const char* name);
};

extern CCollectors Collectors;
#endif

