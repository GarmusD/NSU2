#pragma once

#include "Arduino.h"
#include "Events.h"
#include "Actions.h"
#include "StaticList.h"
#include "consts.h"
#include "ArduinoJson.h"
#include "Settings.h"

class Settings;
class SettingsClass;

class Switch : public Events
{
public:
	Switch(void);
	~Switch(void);
	
	const char* GetName();
	void SwitchState();
	void SetStatus(Status value);
	Status GetStatus();
	
	void PrintStatus();
	String GetInfoString();
	bool IsForced();
	void ApplySettings(byte cfgPos, const SwitchData& data);
	void CleanUp();
private:	
	char name[MAX_NAME_LENGTH];
	byte config_pos;
	Status status, onStatus, forced_status;
	Switch* depend;
	bool isForced;
	void Init();
	void AddDependance(Switch* s, Status OnStatus, Status myStatus);

	void SaveStatus();
	void WriteSettings();
	void HandleStatusChange(void * Sender, Status value);
};


class CSwitches : public StaticList<MAX_SWITCHES_COUNT, Switch>
{
public:
	Switch* GetByName(const char* name);
	void ParseJson(JsonObject& jo);
};

extern CSwitches Switches;