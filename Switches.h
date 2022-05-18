#pragma once

#include "SwitchesDefs.h"
#include "Arduino.h"
#include "aJson.h"
#include "JsonPrinter.h"
#include "Events.h"
#include "Actions.h"
#include "StaticList.h"
#include "consts.h"
#include "ArduinoJson.h"
#include "Defaults.h"
#include "FileManager.h"


class Settings;
class DefaultsClass;

class Switch : public Events
{
public:
	Switch(void);
	~Switch(void);
	
	void Begin();
	const char* GetName();
	void SwitchState();
	void SetStatus(Status value);
	Status GetStatus();
	uint8_t GetConfigPos();
	void PrintStatus();
	bool IsForced();
	void ApplyConfig(uint8_t cfgPos, const SwitchData& data);
	void Reset();
private:	
	char name[MAX_NAME_LENGTH];
	uint8_t configPos;
	Status status, onStatus, forced_status;
	Switch* depend;
	bool isForced, loading, delayedSaveState;
	void Init();
	void AddDependance(Switch* s, Status OnStatus, Status myStatus);

	void SaveStatus();
	void WriteSettings();
	void HandleStatusChange(void * Sender, Status value);
};


class CSwitches : public StaticList<MAX_SWITCHES_COUNT, Switch>, Events
{
public:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;

	Switch* GetByName(const char* name);
	Switch* GetByConfigPos(uint8_t cfgPos);
	void Begin();
	void LoadConfig();
	bool GetConfigData(uint8_t cfgPos, SwitchData& data);
	bool ValidateSetupDataSet(JsonObject & jo);
	void ParseJson(JsonObject & jo);
	void Reset();
private:
	
};

extern CSwitches Switches;