#include "BootChecker.h"

CBootChecker::CBootChecker()
{
	canPauseBoot = true;
}


CBootChecker::~CBootChecker()
{
}

void CBootChecker::CheckBootWait(int seconds)
{
	//StaticJsonBuffer<200> jbuff;
	//JsonObject& root = jbuff.createObject();
	//root[jKeyTarget] = jTargetSystem;
	//root[jKeyAction] = jSysStatusReadyPauseBoot;
	//PrintJson.Print(root);

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
	StaticJsonBuffer<128> jBuffer;
	JsonObject& j = jBuffer.createObject();
	j[jKeyTarget] = jTargetSystem;
	j[jKeyAction] = jSystemActionPauseBoot;
	j[jKeyResult] = jValueResultOk;
	PrintJson.Print(j);
}

void CBootChecker::PrintBootPauseErrorMsg()
{
	StaticJsonBuffer<128> jBuffer;
	JsonObject& j = jBuffer.createObject();
	j[jKeyTarget] = jTargetSystem;
	j[jKeyAction] = jSystemActionPauseBoot;
	j[jKeyResult] = jValueResultError;
	j[jKeyMessage] = "notAllowed";
	PrintJson.Print(j);
}

void CBootChecker::ParseJson(JsonObject& root)
{
	//String action = root[jKeyAction].asString();
	//if (action.equals("pauseboot"))
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