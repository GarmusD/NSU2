#include "JsonPrinter.h"



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
	Log.log("JSON: ", false);
	jo.printTo(Serial);
	Log.newLine();
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
	StaticJsonBuffer<256> jsonBuffer;
	JsonObject& j = jsonBuffer.createObject();
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
	StaticJsonBuffer<128> jsonBuffer;
	JsonObject& j = jsonBuffer.createObject();
	j[jKeyTarget] = target;
	j[jKeyResult] = jValueResultNull;
	if (cmdid[0])
	{
		j[jKeyCmdID] = cmdid;
	}
	Print(j);
}

void JsonPrinter::PrintResultOk(const char * target, const char * action)
{
	StaticJsonBuffer<128> jsonBuffer;
	JsonObject& j = jsonBuffer.createObject();
	j[jKeyTarget] = target;
	if(action && action[0])
		j[jKeyAction] = action;
	j[jKeyResult] = jValueResultOk;
	if (cmdid[0])
	{
		j[jKeyCmdID] = cmdid;
	}
	Print(j);
}

void JsonPrinter::PrintResultDone(const char * target, const char * action)
{
	StaticJsonBuffer<128> jsonBuffer;
	JsonObject& j = jsonBuffer.createObject();
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

void JsonPrinter::PrintResultFormatError()
{
	StaticJsonBuffer<256> jsonBuffer;
	JsonObject& jo = jsonBuffer.createObject();
	jo[jKeyTarget] = "system";
	jo[jKeyAction] = "error";
	jo[jKeyValue] = "format";
	jo[jKeyMessage] = "Missing required fields.";
	Print(jo);
}

void JsonPrinter::PrintResultNotAJson()
{
	StaticJsonBuffer<256> jsonBuffer;
	JsonObject& jo = jsonBuffer.createObject();
	jo[jKeyTarget] = "system";
	jo[jKeyAction] = "error";
	jo[jKeyValue] = "notjson";
	jo[jKeyMessage] = "Not a Json message.";
	Print(jo);
}

void JsonPrinter::PrintResultParseError()
{
	StaticJsonBuffer<256> jsonBuffer;
	JsonObject& jo = jsonBuffer.createObject();
	jo[jKeyTarget] = "system";
	jo[jKeyAction] = "error";
	jo[jKeyValue] = "parse";
	jo[jKeyMessage] = "JSon parse error.";
	Print(jo);
}


JsonPrinter PrintJson;