#ifndef katilas_h
#define katilas_h

#include "Events.h"
#include "List.h"
#include "Logger.h"
#include "TimeSlice.h"
#include "TSensors.h"
#include "MAX31855.h"
#include "RelayModule.h"
#include "Utilities.h"
#include "consts.h"
#include "switches.h"
#include "WaterBoiler.h"
#include "TempTrigger.h"
#include "ArduinoJson.h"
#include "Settings.h"

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

const byte TEMP_ARRAY_SIZE = 20;

class CWoodBoiler: public Events{
public:
	CWoodBoiler(void);
	~CWoodBoiler(void);
	void Begin();
	//static KatilasClass* getInstance();
	void TimeEvent(uint32_t t);
	const char* GetName();
	void SetName(const char* n);
	void SetTempSensor(TempSensor* sensor);
	void SetKTypeSensor(MAX31855* sensor);
	void EnableChimneyTemp(bool enable);
	void SetLadomatChannel(byte idx);
	void SetExhaustFanChannel(byte idx);
	void SetWorkingTemp(float work_temp);
	void SetHisteresis(float temp);
	void SetOperationModeSwitch(Switch* op_mode);
	void SetBurnModeSwitch(Switch* burn_mode);
	void SetLadomatTemp(float ltemp);
	void SetLadomatTrigger(TempTrigger* ltrigger);
	void SetWaterBoiler(CWaterBoiler* b);
	void StartingPhase();
	void EnterPhase1();
	void EnterPhase2();
	void EnterPhase3();
	void EnterPhase4();
	Status GetKatilasStatus();
	Status GetLadomatStatus();
	Status GetSmokeFanStatus();
	float GetCurrentTemp();
	bool IsLadomatasOn();
	bool IsLadomatManual();
	bool IsExhaustFanOn();
	bool IsExhaustFanManual();
	void ChangeExhaustFanManual();
	void ChangeLadomatManual();
	bool SetSimulationMode(bool value);
	bool SetSimulationTemp(float value);
	String GetInfoString();
	String getTempStatusString();
	void ParseJson(JsonObject& jo);
	void PrintWoodBoilerStatus();
	void PrintLadomatStatus();
	void PrintExhaustFanStatus();
	byte FillWoodBoilerData(WoodBoilerData &data);
	void SetSettings(WoodBoilerData &data);
private:
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
	bool ladom_manual;
	uint32_t lastcheck, temp_array_check;
	float working_temp, histeresis, chimneytemp, watertemp, avgwatertemp, ladomattemp;
	uint32_t exhaust_man_start, exhaust_start, ladom_man_start, phase0start, phase2start, phase3start, phase4start, katilasInfoTime;
	AverageF<6> avgf;

	void DoBoilerRoutine();

	void Phase0();
	void Phase1();
	void Phase2();
	void Phase3();
	void Phase4();

	void HandleTemperatureChange(void* Sender, float temp);
	void HandleStatusChange(void* Sender, Status status);
	void OnTimeSlice();

	void SwitchOperationMode(OperationMode new_mode);
	void SwitchBurnMode(BurnMode new_mode);

	void LadomOn();
	void LadomOff();
	void VentOn();
	void VentOff();
	void ChangeExhaustFanStatus(Status newStatus);
	void ChangeLadomatStatus(Status newStatus);
	void ChangeWoodBoilerStatus(Status newStatus);

	void AddTempArray(float value);
};

class CWoodBoilers : public StaticList<MAX_WOOD_BOILERS_COUNT, CWoodBoiler>
{
public:
	CWoodBoiler* GetByName(const char* name);
	void ParseJson(JsonObject& jo);
};

extern CWoodBoilers WoodBoilers;
#endif

