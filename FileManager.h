#pragma once
#ifndef FileManager_h
#define FileManager_h


#include <SdFat.h>
#include "Logger.h"
#include "nsu_pins.h"
#include "ArduinoJson.h"
#include "consts.h"

class CFileManager
{
public:
	CFileManager(const int pin, const uint8_t speed);
	~CFileManager();
	void Init();
	bool Begin();
	bool SDCardOk();
	SdFat& SDFat();

	void ParseJson(JsonObject& jo);
private:
	bool sdOk;
	SdFat sd;
	SdFile file;
	int sdpin;
	uint8_t sdspeed;

	bool isWriting;
	void EnterBlockedWait();
	void ReadFile(JsonObject& jo);
	void WriteFile(JsonObject& jo);

};

extern CFileManager FileManager;
#endif // !FileManager_h