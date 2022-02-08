#pragma once
#ifndef SystemStatus_h
#define SystemStatus_h


#include "Events.h"
#include "Timers.h"
#include "ArduinoJson.h"
#include "aJson.h"
#include "FreeMem.h"
#include "JsonPrinter.h"
//
#include "Settings.h"
#include "TSensors.h"
#include "switches.h"
#include "RelayModule.h"
#include "TempTrigger.h"
#include "CirculationPump.h"

enum ESystemStatus
{
	SystemBooting,
	SystemReadyPauseBoot,
	SystemBootPaused,
	SystemRunning
};

class CSystemStatus: public Events
{
public:
	CSystemStatus();
	~CSystemStatus();
	void begin();
	void SetState(ESystemStatus state);
	void PrintSystemState();
	void Snapshot();
	void ParseJson(JsonObject& jo);
	bool TimeIsSet();
private:
	ESystemStatus currState;
	uint32_t upTime;
	bool timeIsSet;
	void SetDateTime(JsonObject& jo);
	void HandleTimerEvent(int te_id);
};

extern CSystemStatus SystemStatus;
#endif // !SystemStatus_h