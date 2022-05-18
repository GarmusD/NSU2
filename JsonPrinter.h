#pragma once

#include "Arduino.h"
#include "Logger.h"
#include "ArduinoJson.h"
#include "aJson.h"
#include "consts.h"

class JsonPrinter
{
public:
	JsonPrinter();
	~JsonPrinter();
	void Print(JsonObject & jo);
	void setCmdID(const char* id);
	char* getCmdID();
	void PrintResultError(const char* target, const char* errorcode);
	void PrintResultNull(const char* target);
	void PrintResultOk(const char* target, const char * action, bool incl_reboot_req = false);
	void PrintResultDone(const char* target, const char * action);
	void PrintResultFormatError();
	void PrintResultNotAJson();
	void PrintResultParseError(DeserializationError& err);
	void PrintResultUnknownAction(const char* target, const char * action);
private:
	char cmdid[16];
};

extern JsonPrinter PrintJson;