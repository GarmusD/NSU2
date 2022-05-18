#include "JsonPrinter.h"

#include "SystemStatus.h"

JsonPrinter::JsonPrinter()
{
	cmdid[0] = 0;
}


JsonPrinter::~JsonPrinter()
{
}

void JsonPrinter::Print(JsonObject & jo)
{
	if (cmdid[0] && !jo.containsKey(jKeyCmdID))
	{
		jo[jKeyCmdID] = cmdid;
	}
	Serial.print("0 JSON: ");
	//Log.log("JSON: ", false);
	serializeJson(jo, Serial);
	//Log.newLine();
	Serial.print('\n'); //Serial.println(void) prints '\r\n'
	cmdid[0] = 0;
}

void JsonPrinter::setCmdID(const char* id)
{
	strlcpy(cmdid, id, MAX_NAME_LENGTH);
}

char* JsonPrinter::getCmdID()
{
	return cmdid;
}

void JsonPrinter::PrintResultError(const char * target, const char * errorcode)
{
	DynamicJsonDocument jsonBuffer(512);
	JsonObject j = jsonBuffer.to<JsonObject>();
	j[jKeyTarget] = target;
	j[jKeyResult] = jValueResultError;
	j[jKeyMessage] = errorcode;
	if (cmdid[0])
	{
		j[jKeyCmdID] = cmdid;
	}
	Print(j);
}

void JsonPrinter::PrintResultNull(const char * target)
{
	DynamicJsonDocument jsonBuffer(512);
	JsonObject j = jsonBuffer.to<JsonObject>();
	j[jKeyTarget] = target;
	j[jKeyResult] = jValueResultNull;
	if (cmdid[0])
	{
		j[jKeyCmdID] = cmdid;
	}
	Print(j);
}

void JsonPrinter::PrintResultOk(const char * target, const char * action, bool incl_reboot_req)
{
	DynamicJsonDocument jsonBuffer(512);
	JsonObject j = jsonBuffer.to<JsonObject>();
	j[jKeyTarget] = target;
	if(action && action[0])
		j[jKeyAction] = action;
	j[jKeyResult] = jValueResultOk;
	if (cmdid[0])
	{
		j[jKeyCmdID] = cmdid;
	}
	if(incl_reboot_req)
		j[jKeySysStatusRebootRequired] = SystemStatus.GetRebootRequired();
	Print(j);
}

void JsonPrinter::PrintResultDone(const char * target, const char * action)
{
	DynamicJsonDocument jsonBuffer(512);
	JsonObject j = jsonBuffer.to<JsonObject>();
	j[jKeyTarget] = target;
	if (action && action[0])
		j[jKeyAction] = action;
	j[jKeyResult] = jValueResultDone;
	if (cmdid[0])
	{
		j[jKeyCmdID] = cmdid;
	}
	Print(j);
}

void JsonPrinter::PrintResultUnknownAction(const char * target, const char * action)
{
	DynamicJsonDocument jsonBuffer(512);
	JsonObject j = jsonBuffer.to<JsonObject>();
	j[jKeyTarget] = target;
	if (action && action[0])
		j[jKeyAction] = action;
	j[jKeyResult] = jValueResultUnknownAction;
	if (cmdid[0])
	{
		j[jKeyCmdID] = cmdid;
	}
	Print(j);
}

void JsonPrinter::PrintResultFormatError()
{
	DynamicJsonDocument jsonBuffer(512);
	JsonObject jo = jsonBuffer.to<JsonObject>();
	jo[jKeyTarget] = "system";
	jo[jKeyAction] = "error";
	jo[jKeyValue] = "format";
	jo[jKeyMessage] = "Missing required fields.";
	Print(jo);
}

void JsonPrinter::PrintResultNotAJson()
{
	DynamicJsonDocument jsonBuffer(512);
	JsonObject jo = jsonBuffer.to<JsonObject>();
	jo[jKeyTarget] = "system";
	jo[jKeyAction] = "error";
	jo[jKeyValue] = "notjson";
	jo[jKeyMessage] = "Not a Json message.";
	Print(jo);
}

void JsonPrinter::PrintResultParseError(DeserializationError& err)
{
	DynamicJsonDocument jsonBuffer(512);
	JsonObject jo = jsonBuffer.to<JsonObject>();
	jo[jKeyTarget] = "system";
	jo[jKeyAction] = "error";
	jo[jKeyValue] = "parse";
	jo[jKeyMessage] = err.c_str();
	
	Print(jo);
}


JsonPrinter PrintJson;