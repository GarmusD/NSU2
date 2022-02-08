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
	void PrintResultOk(const char* target, const char * action);
	void PrintResultDone(const char* target, const char * action);
	void PrintResultFormatError();
	void PrintResultNotAJson();
	void PrintResultParseError();
private:
	char cmdid[16];
};

extern JsonPrinter PrintJson;