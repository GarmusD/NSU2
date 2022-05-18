#include "CmdServer.h"
#include "FreeMem.h"
#include "advancedFunctions.h"
//#include "windows.h"
#include <ArduinoJson.h>
#include "aJson.h"

#include "FileManager.h"
#include "Defaults.h"
#include "TSensors.h"
#include "switches.h"
#include "RelayModules.h"
#include "TempTriggers.h"
#include "CirculationPumps.h"
#include "Collectors.h"
#include "ComfortZones.h"
#include "WaterBoilers.h"
#include "WoodBoilers.h"
#include "SystemFans.h"
#include "Alarms.h"


char cmdID[16];
char action[16];

void OnMessage()
{
	cmdServer.ParseMessage();
}

void OnMessageOneId()
{

}

void OnMessageTwoId()
{

}

CmdServer::CmdServer(Stream & comms, const char fld_separator/* = ','*/,
	const char cmd_separator/* = ';'*/,
	const char esc_character/* = '/'*/) :messenger(comms, fld_separator, cmd_separator, esc_character)
{
	messenger.printLfCr(true);
	cmdID[0] = 0;
	action[0] = 0;
}

CmdMessenger& CmdServer::getCmdMessenger()
{
	return messenger;
}

void CmdServer::begin()
{
	TimeSlice.RegisterTimeSlice(this);
	while (Serial.available())
	{
		Serial.read();
	}

	messenger.attach(OnMessage);
}

CmdServer::~CmdServer(void)
{
}

void CmdServer::OnTimeSlice()
{
	messenger.feedinSerialData();
}

void CmdServer::ParseMessage()
{
	//if (messenger == NULL) return;
	const char* x = messenger.readStringArg();
	//Log.debug("Msg is: ["+x+"]");
	
	//if msg is JSON format
	if(x && x[0] == '{')//x.startsWith("{"))
	{
		Log.debug("Msg is JSON.");
		ParseJsonMessage(x);
		return;
	}
	Log.debug("Msg is NOT JSON.");
	PrintJson.PrintResultNotAJson();
}

void CmdServer::ParseJsonMessage(const char* json)
{
	DynamicJsonDocument jsonBuffer(JsonBuffSize);
	DeserializationError error = deserializeJson(jsonBuffer, json);
	
	if (error)
	{
		PrintJson.PrintResultParseError(error);
		return;
	}
	JsonObject jo = jsonBuffer.as<JsonObject>();

	if (!jo.containsKey(jKeyTarget))
	{
		PrintJson.PrintResultFormatError();
		return;

	}
	const char* target = jo[jKeyTarget];

	//save for response
	if (jo.containsKey(jKeyCmdID))
	{
		PrintJson.setCmdID(jo[jKeyCmdID]);
	}
	else
	{
		PrintJson.setCmdID("");
	}

	char s[128];
	sprintf(s, "CmdServer::ParseJsonMessage() Target - %s", target);
	Log.debug(s);
	if (strcmp(target, jTargetBoot) == 0)
	{
		BootChecker.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetSystem) == 0)
	{
		SystemStatus.ParseJson(jo);
		return;
	}
	if (strcmp(target, jKeyTargetTSensor) == 0)
	{
		TSensors.ParseJSON(jo);
		return;
	}
	if (strcmp(target, jTargetSwitch) == 0)
	{
		Switches.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetRelay) == 0)
	{
		RelayModules.ParseJSON(jo);
		return;
	}
	if (strcmp(target, jTargetTempTrigger) == 0)
	{
		TempTriggers.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetCircPump) == 0)
	{
		CirculationPumps.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetCollector) == 0)
	{
		Collectors.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetComfortZone) == 0)
	{
		ComfortZones.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetKType) == 0)
	{
		KTypes.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetWaterBoiler) == 0)
	{
		WaterBoilers.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetWoodBoiler) == 0)
	{
		WoodBoilers.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetSystemFan) == 0)
	{
		SysFans.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetFileManager) == 0)
	{
		FileManager.ParseJson(jo);
		return;
	}
	if (strcmp(target, jTargetAlarm) == 0)
	{
		Alarms.ParseJson(jo);
		return;
	}

	sprintf(s, "CmdServer::ParseJsonMessage(). Target [%s] not found.", target);
	Log.debug(s);
}


CmdServer cmdServer(Serial, ' ', '\n');
