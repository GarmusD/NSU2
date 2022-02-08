#pragma once

#include "Logger.h"
#include "Events.h"
#include "consts.h"
#include "TimeSlice.h"
#include "TSensors.h"
#include "ArduinoJson.h"
#include "JsonPrinter.h"
#include "aJson.h"

class SystemFan: public Events
{
public:
	SystemFan();
	~SystemFan();
	void Begin();
	void SetTempSensor(TempSensor* sensor);
	void SetTemperatures(float min, float max);
	void ParseJSON(JsonObject & jo);
private:
	float mint, maxt;
	TempSensor* ts;
	void OnTimeSlice();
	void HandleTemperatureChange(void* Sender, float value);
};

extern SystemFan SysFan;