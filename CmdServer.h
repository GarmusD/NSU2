#pragma once
#include "CmdMessenger.h"
//#include <Messenger.h>
#include "Events.h"
#include "TimeSlice.h"
#include "Logger.h"
#include "JsonPrinter.h"
#include "BootChecker.h"
#include "SystemStatus.h"

class CmdServer:public Events
{
public:
	CmdServer(Stream & comms, const char fld_separator = ',', 
				const char cmd_separator = ';', 
                const char esc_character = '/');
	~CmdServer(void);
	void begin();
	void ParseMessage();
	void ParseJsonMessage(const char* json);
	//void PrintJSON(JsonObject jo);
	void OnTimeSlice();
	CmdMessenger & getCmdMessenger();
private:
	CmdMessenger messenger;	
	//bool bootWaitDone;	
};

extern CmdServer cmdServer;