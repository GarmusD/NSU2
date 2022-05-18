#ifndef CirculationPump_h
#define CirculationPump_h

#include "CirculationPumpsDefs.h"
#include "consts.h"
#include <WString.h>
#include "StaticList.h"
#include "Logger.h"
#include "RelayModules.h"
#include "Events.h"
#include "TempTriggers.h"
#include "Timers.h"
#include "Defaults.h"
#include "SystemStatus.h"

class CirculationPump: public Events
{
public:
	CirculationPump(void);
	void Init();
	~CirculationPump(void);
	void ValveOpened(void);
	void ValveClosed(void);
	void StartPump(void);
	void StopPump(void);
	void DisablePump(void);
	bool IsRunning(void);
	bool IsEnabled(void);
	void SwitchManualMode(void);
	uint8_t GetConfigPos(void);
	const char* GetName(void);
	void SetSpeed(uint8_t maxspeed, uint8_t speed1_channel, uint8_t speed2_channel, uint8_t speed3_channel);
	void RegisterValveCount(uint8_t valvecount);
	void SetTempTrigger(TempTrigger* value);
	Status getStatus(void);
	int GetOpenedValveCount(void);
	int GetCurrentSpeed(void);
	void ApplyConfig(uint8_t cfgPos, const CircPumpData& data);
	void Reset(void);
private:
	uint8_t configPos;
	Status status;
	char name[MAX_NAME_LENGTH];
	TempTrigger* trigger;
	uint8_t channel;
	int counter;
	bool running;
	bool manual;
	bool enabled;//
	bool standalone;
	uint8_t max_speed;
	uint8_t current_speed;
	uint8_t speed_table[3];
	uint8_t speed_channels[3];
	uint8_t total_valves;

	void SetName(const char* name);
	void ChangeStatus(Status new_status);
	void PrintStatus(const char * action = "");
	void calc_speed_table(void);
	void switch_speed(void);
	uint8_t getReqSpeed(void);	
	void HandleStatusChange(void* Sender, Status value);
	void HandleTimerEvent(int te_id);
};

class CCirculationPumps: public StaticList<MAX_CIRCULATION_PUMPS, CirculationPump>
{
public:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;

	CirculationPump* GetByName(const char* name);
	CirculationPump* GetByConfigPos(uint8_t cfgPos);
	void LoadConfig(void);
	bool GetConfigData(uint8_t cfgPos, CircPumpData & data);
	bool ValidateSetupDataSet(JsonObject& jo);
	void ParseJson(JsonObject& jo);
	void Reset();
};

extern CCirculationPumps CirculationPumps;

#endif

