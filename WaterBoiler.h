#ifndef WaterBoiler_h
#define WaterBoiler_h

#include "Events.h"
#include "Logger.h"
//#include "WoodBoiler.h"
#include "TempTrigger.h"
#include "switches.h"
#include "CirculationPump.h"
#include "TSensors.h"
#include "RelayModule.h"

struct ElHeatingData
{
	byte StartHour;
	byte StartMin;
	byte EndHour;
	byte EndMin;
};

const byte MAX_WATERBOILER_COUNT = 1;
const byte MAX_WATERBOILER_EL_HEATING_DATA_COUNT = 7;

class CWaterBoiler:public Events
{
public:
	CWaterBoiler(void);
	~CWaterBoiler(void);
	void Begin();
	void SetName(char* n);
	char* GetName();
	void SetElPowerChannel(byte value);
	void SetElPowerEnabled(bool value);
	void SetElPowerData(byte pos, ElHeatingData data);
	void SetTempTrigger(TempTrigger* value);
	void SetCircPump(CirculationPump* value);
	void SetEnabled(bool value);
	void StartCircPump();
	void StopCircPump();
	void SetExternalControl(bool value);
private:
	char name[33];
	bool enabled, epenabled, epoweron, extcontrol;
	ElHeatingData epdataa[MAX_WATERBOILER_EL_HEATING_DATA_COUNT], epdata;
	byte epowerchannel;
	TempTrigger* trigger;
	Switch* swth;
	CirculationPump* cpump;

	bool TimeInRange(uint8_t sh, uint8_t sm, uint8_t eh, uint8_t em, uint8_t ch, uint8_t cm);

	//bool CheckTempTriggers();
	void HandleStatusChange(void* Sender, Status status);
	void HandleTimerEvent(int te_id);
};

class CWaterBoilers : public StaticList<MAX_WATERBOILER_COUNT, CWaterBoiler>
{
public:
	CWaterBoiler* GetByName(const char * n);
};

extern CWaterBoilers WaterBoilers;

#endif
