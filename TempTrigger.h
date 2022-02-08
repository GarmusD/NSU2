#ifndef TempTrigger_h
#define TempTrigger_h

#include "Events.h"
#include "StaticList.h"
#include "TSensors.h"
#include "consts.h"

const int MAX_TEMP_TRIGGERS = 16;
const int MAX_TEMP_TRIGGER_PIECES = 4;

enum TriggerCondition
{
	TrueIfLower,
	TrueIfHigher
};

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
	void SetName(const char* value);
	const char* GetName();
	Status GetStatus();
	String GetInfoString();
	void PrintInfo();
private:
	char name[MAX_NAME_LENGTH];
	Status status;
	StaticList<MAX_TEMP_TRIGGER_PIECES, TempTriggerPiece> pieces;
	void ChangeStatus(Status value);
	void HandleStatusChange(void* Sender, Status value);
};

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

class CTempTriggers: public StaticList<MAX_TEMP_TRIGGERS, TempTrigger>
{
public:
	TempTrigger* GetByName(const char* value);
};

extern CTempTriggers TempTriggers;

#endif

