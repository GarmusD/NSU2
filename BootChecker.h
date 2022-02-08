#pragma once
#ifndef BootChecker_h
#define BootChecker_h


#include "SdFat.h"
#include "CmdServer.h"
#include "ArduinoJson.h"
#include "SystemStatus.h"
#include "aJson.h"

class CBootChecker
{
public:
	CBootChecker();
	~CBootChecker();
	void CheckBootWait(int seconds);
	void PauseBootRequested();
	void ContinueBoot();
	void ParseJson(JsonObject& root);
private:
	SdFile file;
	bool canPauseBoot;
	bool enterPauseBoot;
	bool bootPaused;
	bool breakCheckBootWait;
	void proccessPausedBoot();
	void PrintBootPausedMsg();
	void PrintBootPauseErrorMsg();
};

extern CBootChecker BootChecker;
#endif // !BootChecker_h