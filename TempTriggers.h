#ifndef TempTrigger_h
#define TempTrigger_h

#include "TempTriggersDefs.h"
#include "Events.h"
#include "StaticList.h"
#include "TSensors.h"
#include "consts.h"
#include "Defaults.h"
#include "aJson.h"



/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

class TempTriggerPiece:public Events
{
public:
	TempTriggerPiece(void);
	~TempTriggerPiece(void);

	void SetTriggerCondition(TriggerCondition cond);
	void SetTempSensor(TempSensor* sensor);
	void SetTemperature(float value);
	void SetHisteresis(float value);
	bool GetStatus();
	void Reset();
private:
	float req_temp, sens_temp, histeresis;
	bool state;
	TempSensor* tsensor;
	TriggerCondition condition;
	void HandleTemperatureChange(void* Sender, float value);	
};

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

class TempTrigger: public Events
{
public:
	TempTrigger(void);
	~TempTrigger(void);

	bool Check();
	void AddTriggerPiece(TriggerCondition cond, const char* tsensname, float temperature, float hister);
	void PiecesDone();
	uint8_t GetConfigPos();
	const char* GetName();
	Status GetStatus();
	void PrintInfo();
	void ApplyConfig(uint8_t cfgPos, const TempTriggerData& data);
	void Reset();
private:
	uint8_t configPos;
	char name[MAX_NAME_LENGTH];
	Status status;
	StaticList<MAX_TEMP_TRIGGER_PIECES, TempTriggerPiece> pieces;
	void SetName(const char* value);
	void ChangeStatus(Status value);
	void HandleStatusChange(void* Sender, Status value);
};

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

class CTempTriggers: public StaticList<MAX_TEMP_TRIGGERS, TempTrigger>, Events
{
public:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;

	TempTrigger* GetByName(const char* value);
	TempTrigger* GetByConfigPos(uint8_t cfgPos);
	void Reset();
	void LoadConfig();
	bool GetConfigData(uint8_t cfgPos, TempTriggerData & data);
	bool ValidateSetupDataSet(JsonObject & jo);
	void ParseJson(JsonObject & jo);
};

extern CTempTriggers TempTriggers;

#endif

