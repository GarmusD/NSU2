#pragma once

#include "AlarmsDefs.h"
#include "ArduinoJson.h"
#include "JsonPrinter.h"
#include "RelayModules.h"
#include "FileManager.h"
#include "SystemStatus.h"

class CAlarm
{
public:
	CAlarm();
	~CAlarm();
	bool IsAlarm(void);
	void SetCurrentTemperature(float temp);
	void Reset(void);
	void ApplyConfig(uint8_t cfgPos, const AlarmData& data);
	uint8_t GetConfigPos();
private:	
	uint8_t configPos;
	AlarmChannelInfo achInfo[MAX_ALARM_CHANNELS];
	float alarmTemp, histersis;
	bool isAlarm;
	
};

class CAlarms : public StaticList<MAX_ALARM_COUNT, CAlarm>
{
public:
	CAlarm* GetByConfigPos(uint8_t cfgPos);
	bool ValidateSetupDataSet(JsonObject & jo);
	void LoadConfig(void);
	bool GetConfigData(uint8_t pos, AlarmData& data);
	void ParseJson(JsonObject& jo);
	void Reset(void);
private:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;
};

extern CAlarms Alarms;