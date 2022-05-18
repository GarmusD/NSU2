#pragma once

#include "SystemFansDefs.h"
#include "Logger.h"
#include "Events.h"
#include "consts.h"
#include "TimeSlice.h"
#include "TSensors.h"
#include "ArduinoJson.h"
#include "JsonPrinter.h"
#include "aJson.h"
#include "Defaults.h"
#include "SystemStatus.h"

class SystemFan: public Events
{
public:
	SystemFan();
	~SystemFan();
	void Begin();
	void SetTempSensor(TempSensor* sensor);
	void SetTemperatures(float min, float max);
	char* GetName();
	uint8_t GetConfigPos();
	uint8_t GetCurrentPWM();	
	void ParseJSON(JsonObject & jo);
	void ApplyConfig(uint8_t cfgPos, const SystemFanData& data);
	void Reset();
private:
	uint8_t configPos;
	char name[MAX_NAME_LENGTH];
	uint8_t currentPWM;
	float mint, maxt;
	TempSensor* ts;
	void SetName(const char* n);
	void PrintStatus();
	void OnTimeSlice();
	void HandleTemperatureChange(void* Sender, float value);
};

class SystemFans : public StaticList<MAX_SYSTEM_FAN_COUNT, SystemFan>
{
public:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;

	SystemFan* GetByName(const char * n);
	SystemFan* GetByConfigPos(int cfgPos);
	void Begin();
	void Reset();
	void LoadConfig();
	bool GetConfigData(uint8_t cfgPos, SystemFanData & data);
	bool ValidateSetupDataSet(JsonObject &jo);
	void ParseJson(JsonObject &jo);
};

extern SystemFans SysFans;