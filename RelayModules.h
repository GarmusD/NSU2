#ifndef RelayModule_h
#define RelayModule_h

#include "RelayModulesDefs.h"
#include <Arduino.h>
#include "Logger.h"
#include "Events.h"
#include "TimeSlice.h"
#include "aJson.h"
#include "ArduinoJson.h"
#include "JsonPrinter.h"
#include "Defaults.h"
#include "StaticList.h"
#include "FileManager.h"

struct RelayModuleInfo
{
	bool activelow;
	bool inverted;
	uint8_t flags;
	uint8_t lock;
	uint8_t uflags;
	void Reset();
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
	void InitHW();
	void Begin();
	
	bool OpenChannel(uint8_t channel);
	bool CloseChannel(uint8_t channel);
	bool GetChannelStatus(uint8_t channel);
	void AllChannelsOff();
	void LockChannel(uint8_t ch, bool value);
	void UnlockChannel(uint8_t ch);
	void ParseJSON(JsonObject & jo);
	uint8_t GetFlags(uint8_t module_idx);
	void LoadConfig();
	bool GetConfigData(uint8_t cfgPos, RelayModuleData & data);
	void Reset();
	//void OnTimeSlice();
	void WriteChannels();
protected:
	
private:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;
	StaticList<MAX_RELAY_MODULES, RelayModuleInfo> modules;
	//void WriteChannels();
	//RelayModuleType mtype;
	uint8_t* flags;
	uint8_t flag;
	bool update_pending;
	uint32_t last_rewrite;

	bool ValidateSetupDataSet(JsonObject & jo);
	void AddRelayModule(uint8_t activelow, uint8_t inverted);
	void OnTimeSlice();
	void PrintChannelAction(uint8_t ch, bool open, bool locked = false);
};

extern CRelayModules RelayModules;

#endif

