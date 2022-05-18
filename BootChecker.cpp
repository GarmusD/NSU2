#include "BootChecker.h"

CBootChecker::CBootChecker()
{
	canPauseBoot = true;
}


CBootChecker::~CBootChecker()
{
}

void CBootChecker::Reset()
{
	canPauseBoot = true;
}

void CBootChecker::CheckBootWait(int seconds)
{
	SystemStatus.SetState(SystemReadyPauseBoot);

	uint32_t finish = millis() + (seconds * 1000);
	breakCheckBootWait = false;
	while (finish > millis())
	{
		if (breakCheckBootWait)
		{
			break;
		}
		cmdServer.OnTimeSlice();		
	}
	canPauseBoot = false;
	if (enterPauseBoot)
	{
		proccessPausedBoot();
	}
}

void CBootChecker::PauseBootRequested()
{
	//Break waiting anyway, error or not...
	breakCheckBootWait = true;
	if (canPauseBoot)
	{
		enterPauseBoot = true;
	}
	else
	{	
		PrintBootPauseErrorMsg();
	}
}

void CBootChecker::PrintBootPausedMsg()
{
	DynamicJsonDocument jBuffer(512);
	JsonObject j = jBuffer.to<JsonObject>();
	j[jKeyTarget] = jTargetSystem;
	j[jKeyAction] = jKeySystemActionPauseBoot;
	j[jKeyResult] = jValueResultOk;
	PrintJson.Print(j);
}

void CBootChecker::PrintBootPauseErrorMsg()
{
	DynamicJsonDocument jBuffer(512);
	JsonObject j = jBuffer.to<JsonObject>();
	j[jKeyTarget] = jTargetSystem;
	j[jKeyAction] = jKeySystemActionPauseBoot;
	j[jKeyResult] = jValueResultError;
	j[jKeyMessage] = "notAllowed";
	PrintJson.Print(j);
}

void CBootChecker::ParseJson(JsonObject root)
{
	//const char* action = root[jKeyAction].asString();
	//if (strcmp(action, "pauseboot") == 0)
	//{
	//	
	//}
}

void CBootChecker::proccessPausedBoot()
{
	bootPaused = true;
	SystemStatus.SetState(SystemBootPaused);
	while (bootPaused)
	{
		cmdServer.OnTimeSlice();
	}
	SystemStatus.SetState(SystemBooting);
}

void CBootChecker::ContinueBoot()
{
	bootPaused = false;
}

CBootChecker BootChecker;