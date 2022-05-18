#ifndef Collector_h
#define Collector_h

#include "CollectorsDefs.h"
#include "StaticList.h"
#include "Events.h"
#include "CirculationPumps.h"
#include "RelayModules.h"
#include "JsonPrinter.h"
#include "Defaults.h"
#include "FileManager.h"

class Collector:public Events
{
public:
	Collector(void);
	void Init(void);
	~Collector(void);
	void OpenValve(uint8_t valve);
	void CloseValve(uint8_t valve);	
	const char* GetName(void);
	uint8_t GetValveCount(void);
	bool GetValveStatus(uint8_t pos);
	uint8_t GetConfigPos(void);
	void PrintStatus(void);
	void ApplyConfig(uint8_t cfgPos, const CollectorData& data);
	void Reset(void);
private:
	

	uint8_t configPos;
	char name[MAX_NAME_LENGTH];
	int max_count, count;
	bool pump_enabled, channels_closed;
	CirculationPump* pump;
	//RelayModule* rm;
	ThermoValve valves[MAX_COLLECTOR_VALVES];
	bool valve_status[MAX_COLLECTOR_VALVES];

	void SetValveCount(uint8_t value);
	void SetThermoValve(uint8_t idx, const ThermoValve& valve);
	void SetCirculationPump(CirculationPump* circpump);
	void SetName(const char* name);
	void HandleStatusChange(void* Sender, Status status);
	void CloseRelayChannels(void);
	void OpenRelayChannels(void);
};

class CCollectors: public StaticList<MAX_COLLECTORS, Collector>
{
public:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;

	Collector* GetByName(const char* name);
	Collector* GetByConfigPos(uint8_t cfgPos);
	void Reset();
	void LoadConfig();
	bool GetConfigData(uint8_t cfgPos, CollectorData & data);
	bool ValidateSetupDataSet(JsonObject& jo);
	void ParseJson(JsonObject& jo);
};

extern CCollectors Collectors;
#endif

