#pragma once
#ifndef SystemStatus_h
#define SystemStatus_h

#include "BootChecker.h"

#include "Events.h"
#include "Timers.h"
#include "ArduinoJson.h"
#include "aJson.h"
#include "FreeMem.h"
#include "JsonPrinter.h"
//

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
	void ParseJson(JsonObject & jo);
	bool TimeIsSet();
	bool GetRebootReqested();
	bool GetRebootRequired();
	void SetRebootRequired();
	void Reset();
private:
	ESystemStatus currState;
	uint32_t upTime;
	bool timeIsSet;
	bool rebootReqested;
	bool rebootRequired;
	void SetDateTime(JsonObject jo);
	void HandleTimerEvent(int te_id);
};

extern CSystemStatus SystemStatus;
#endif // !SystemStatus_h