#pragma once
#ifndef FileManager_h
#define FileManager_h


#include <SdFat.h>
#include "Logger.h"
#include "nsu_pins.h"
#include "ArduinoJson.h"
#include "consts.h"

struct VersionInfo
{
	char vstring[3];
	int version;
};

enum FileStatus
{
	FileStatus_OK,	
	FileStatus_ReCreate,
	FileStatus_RWError,
	FileStatus_Inaccessible
};

class CFileManager
{
public:
	CFileManager(const int pin, const uint8_t speed=0);
	~CFileManager();
	void InitHW();
	bool Begin();
	bool SDCardOk();
	SdFat& SDFat();
	bool FileExists(const char* fname);
	bool FileOpen(const char* fname);
	bool FileCreate(const char* fname);
	bool FileTruncate();
	bool FileClose();
	bool FileSeek(uint32_t pos);
	uint8_t FileReadByte();
	int32_t FileReadInt();
	bool FileReadBuffer(void* buff, size_t length);
	bool FileWriteByte(uint8_t val);
	bool FileWriteInt(int32_t val);
	bool FileWriteBuff(const void* buff, size_t length);

	FileStatus OpenConfigFile(const char* fname, const VersionInfo& vi);

	void ParseJson(JsonObject jo);
private:
	bool sdOk;
	SdFat sd;
	SdFile file;
	bool fileWasWritted;
	int sdpin;
	uint8_t sdspeed;

	bool isWriting;
	void EnterBlockedWait();
	void ReadFile(JsonObject jo);
	void WriteFile(JsonObject jo);

};

extern CFileManager FileManager;
#endif // !FileManager_h