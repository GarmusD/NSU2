#ifndef CirculationPump_h
#define CirculationPump_h

#include <WString.h>
#include "StaticList.h"
#include "Logger.h"
#include "RelayModule.h"
#include "Events.h"
#include "TempTrigger.h"
#include "Timers.h"

const int MAX_CIRCULATION_PUMPS = 8;
const int MAX_CIRCULATION_PUMP_NAME = 32;

class CirculationPump: public Events
{
public:
	CirculationPump(void);
	~CirculationPump(void);
	void ValveOpened();
	void ValveClosed();
	void StartPump();
	void StopPump();
	void DisablePump();
	bool IsRunning();
	bool IsEnabled();
	void SetRelayChannel(int ch);
	void SwitchManualMode();
	void SetName(const char* name);
	void SetStandalone(bool value);
	const char* GetName();
	void SetMaxSpeed(byte maxspeed, byte speed1_channel, byte speed2_channel, byte speed3_channel);
	void RegisterValveCount(byte valvecount);
	void SetTempTrigger(TempTrigger* value);
	Status getStatus();
	String GetInfoString();
	int GetOpenedValveCount();
	int GetCurrentSpeed();
private:
	Status status;
	char name[MAX_NAME_LENGTH];
	//RelayModule* rm;
	TempTrigger* trigger;
	int channel;
	int counter;
	bool running;
	bool manual;
	bool enabled;//
	bool standalone;
	byte max_speed;
	byte current_speed;
	byte speed_table[3];
	byte speed_channels[3];
	byte total_valves;

	void ChangeStatus(Status new_status);
	void PrintStatus(String action = "");
	void calc_speed_table();
	void switch_speed();
	byte getReqSpeed();
	//bool CheckTempTriggers();
	void HandleStatusChange(void* Sender, Status value);
	void HandleTimerEvent(int te_id);
};

class CCirculationPumps: public StaticList<MAX_CIRCULATION_PUMPS, CirculationPump>
{
public:
	CirculationPump* GetByName(const char* name);
	void ParseJson(JsonObject& jo);
};

extern CCirculationPumps CirculationPumps;

#endif

