#ifndef RelayModule_h
#define RelayModule_h

#include <Arduino.h>
#include "Logger.h"
#include "Events.h"
#include "TimeSlice.h"
#include "aJson.h"
#include "ArduinoJson.h"
#include "JsonPrinter.h"

const int MAX_RELAY_MODULES = 5;

struct RelayModuleInfo
{
	bool activelow;
	bool inverted;
	byte flags;
};

/*
enum RelayModuleType
{
	RelayUnknown,
	Relay8,
	Relay16,
	Relay24
};
*/

class CRelayModules:public Events
{
public:
	CRelayModules(void);
	~CRelayModules(void);
	void Init();
	void Begin();
	void AddRelayModule(byte activelow, byte inverted);
	bool OpenChannel(byte channel);
	bool CloseChannel(byte channel);
	String GetInfoString();
	bool GetChannelStatus(byte channel);
	void AllChannelsOff();
	void ParseJSON(JsonObject & jo);
	byte GetFlags(byte module_idx);
protected:
	
private:
	//static RelayModule* _instance;
	StaticList<MAX_RELAY_MODULES, RelayModuleInfo> modules;
	void WriteChannels();
	//RelayModuleType mtype;
	byte channelCount;
	byte* flags;
	byte flag;
	bool update_pending;
	uint32_t last_rewrite;

	void OnTimeSlice();
	void PrintChannelAction(byte ch, bool open);
};

extern CRelayModules RelayModules;

#endif

