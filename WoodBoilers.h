#ifndef katilas_h
#define katilas_h

#include "WoodBoilersDefs.h"
#include "Events.h"
#include "List.h"
#include "Logger.h"
#include "TimeSlice.h"
#include "TSensors.h"
#include "MAX31855.h"
#include "RelayModules.h"
#include "Utilities.h"
#include "consts.h"
#include "switches.h"
#include "WaterBoilers.h"
#include "TempTriggers.h"
#include "ArduinoJson.h"
#include "Defaults.h"
#include "Alarms.h"

const char* const cTS_GrowingFast = "GrowingFast";
const char* const cTS_Growing = "Growing";
const char* const cTS_Stable = "Stable";
const char* const cTS_Lowering = "Lowering";
const char* const cTS_LoweringFast = "LoweringFast";
const char* const cTS_Default = "Unknown";

enum OperationMode{
	WinterMode,
	SummerMode
};

enum BurnMode{
	EcoMode,
	NormalMode
};

enum TempStatus{
	TS_GrowingFast,
	TS_Growing,
	TS_Stable,
	TS_Lowering,
	TS_LoweringFast
};

const uint8_t TEMP_ARRAY_SIZE = 20;

class CWoodBoiler: public Events{
public:
	CWoodBoiler(void);
	~CWoodBoiler(void);
	void Begin(void);
	const char* GetName(void);
	void SetName(const char* n);
	void SetTempSensor(TempSensor* sensor);
	void SetKTypeSensor(MAX31855* sensor);
	void EnableChimneyTemp(bool enable);
	void SetLadomatChannel(uint8_t idx);
	void SetExhaustFanChannel(uint8_t idx);
	void SetWorkingTemp(float work_temp);
	void SetHisteresis(float temp);
	void SetOperationModeSwitch(Switch* op_mode);
	void SetBurnModeSwitch(Switch* burn_mode);
	void SetLadomatTemp(float ltemp);
	void SetLadomatTrigger(TempTrigger* ltrigger);
	void SetWaterBoiler(CWaterBoiler* b);
	void StartingPhase(void);
	void EnterPhase1(void);
	void EnterPhase2(void);
	void EnterPhase3(void);
	void EnterPhase4(void);
	Status GetKatilasStatus(void);
	Status GetLadomatStatus(void);
	Status GetSmokeFanStatus(void);
	float GetCurrentTemp(void);
	bool IsLadomatasOn(void);
	bool IsLadomatManual(void);
	bool IsExhaustFanOn(void);
	bool IsExhaustFanManual(void);
	void ChangeExhaustFanManual(void);
	void ChangeLadomatManual(void);
	bool SetSimulationMode(bool value);
	bool SetSimulationTemp(float value);
	const char* getTempStatusString(void);
	void ParseJson(JsonObject jo);
	void PrintWoodBoilerStatus(void);
	void PrintLadomatStatus(void);
	void PrintExhaustFanStatus(void);
	void ApplyConfig(uint8_t cfgPos, const WoodBoilerData &data);
	void Reset();
private:
	uint8_t configPos;
	char name[33];
	bool simulation;
	float sim_orig_temp;
	uint32_t sim_start;
	TempStatus temp_status;
	float temp_array[TEMP_ARRAY_SIZE];
	bool temp_array_add;
	OperationMode opmode;
	BurnMode burnmode;
	Status status, ladomat_status, exhaustfan_status;
	MAX31855* chimneytempsens;
	bool chimneytem_enabled;
	TempSensor* watertempsens;
	Switch* opmodeswitch;
	Switch* burnmodeswitch;
	TempTrigger* ladomat_trigger;
	CWaterBoiler* waterboiler;
	Status ladomat_trigger_status;

	int ladom_channel;
	int exhaustfan_channel;
	bool ladom_on;
	bool exhaust_on;
	bool exhaust_manual;
	bool exhaust_ctrl_enabled, exhaust_disabled, exhaust_last_state;
	float exhaust_temp, exhaust_hist;
	bool ladom_manual;
	uint32_t lastcheck, temp_array_check;
	float working_temp, histeresis, chimneytemp, watertemp, avgwatertemp, ladomattemp;
	uint32_t exhaust_man_start, exhaust_start, ladom_man_start, phase0start, phase2start, phase3start, phase4start, katilasInfoTime;
	AverageF<6> avgf;

	void DoBoilerRoutine(void);

	void Phase0(void);
	void Phase1(void);
	void Phase2(void);
	void Phase3(void);
	void Phase4(void);

	void HandleTemperatureChange(void* Sender, float temp);
	void HandleStatusChange(void* Sender, Status status);
	void OnTimeSlice(void);

	void SwitchOperationMode(OperationMode new_mode);
	void SwitchBurnMode(BurnMode new_mode);

	void LadomOn(void);
	void LadomOff(void);
	void VentOn(void);
	void VentOff(void);
	void ChangeExhaustFanStatus(Status newStatus);
	void ChangeLadomatStatus(Status newStatus);
	void ChangeWoodBoilerStatus(Status newStatus);

	void AddTempArray(float value);
};

class CWoodBoilers : public StaticList<MAX_WOOD_BOILERS_COUNT, CWoodBoiler>
{
public:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;

	CWoodBoiler* GetByName(const char* name);
	void Begin(void);
	void Reset(void);
	void LoadConfig(void);
	bool GetConfigData(uint8_t cfgPos, WoodBoilerData & data);
	bool ValidateSetupDataSet(JsonObject& jo);
	void ParseJson(JsonObject& jo);
};

extern CWoodBoilers WoodBoilers;
#endif

