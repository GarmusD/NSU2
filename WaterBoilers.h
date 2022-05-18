#ifndef WaterBoiler_h
#define WaterBoiler_h

#include "WaterBoilersDefs.h"
#include "Events.h"
#include "Logger.h"
#include "TempTriggers.h"
#include "switches.h"
#include "CirculationPumps.h"
#include "TSensors.h"
#include "RelayModules.h"


class CWaterBoiler:public Events
{
public:
	CWaterBoiler(void);
	~CWaterBoiler(void);
	void Begin(void);
	
	char* GetName(void);	
	void StartCircPump(void);
	void StopCircPump(void);
	void SetExternalControl(bool value);
	void ApplyConfig(uint8_t cfgPos, const WaterBoilerData& data);
	void Reset(void);
private:
	uint8_t configPos;
	char name[MAX_NAME_LENGTH];
	bool enabled, epenabled, epoweron, extcontrol;
	ElHeatingData epdataa[MAX_WATERBOILER_EL_HEATING_DATA_COUNT], epdata;
	uint8_t epowerchannel;
	TempTrigger* trigger;
	Switch* swth;
	CirculationPump* cpump;

	void SetName(const char* n);
	void SetElPowerEnabled(bool value);
	void SetElPowerData(uint8_t pos, ElHeatingData data);
	void SetTempTrigger(TempTrigger* value);
	void SetCircPump(CirculationPump* value);

	bool TimeInRange(uint8_t sh, uint8_t sm, uint8_t eh, uint8_t em, uint8_t ch, uint8_t cm);

	//bool CheckTempTriggers();
	void HandleStatusChange(void* Sender, Status status);
	void HandleTimerEvent(int te_id);
};

class CWaterBoilers : public StaticList<MAX_WATERBOILER_COUNT, CWaterBoiler>
{
public:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;

	CWaterBoiler* GetByName(const char * n);
	void Begin(void);
	void Reset(void);
	void LoadConfig();
	bool GetConfigData(uint8_t cfgPos, WaterBoilerData & data);
	bool ValidateSetupDataSet(JsonObject& jo);
	void ParseJson(JsonObject& jo);
};

extern CWaterBoilers WaterBoilers;

#endif
