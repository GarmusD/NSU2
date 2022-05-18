#ifndef MAX31855_h
#define MAX31855_h

#include "MAX31855Defs.h"
#include "FileManager.h"
#include "Logger.h"
#include "Events.h"
#include "StaticList.h"
#include "utilities.h"
#include "Timers.h"
#include "consts.h"
#include "TimeSlice.h"
#include "Defaults.h"

class MAX31855:public Events
{
public:
	MAX31855(void);
	~MAX31855(void);	
	void Begin();
	int32_t GetTemp();	
	const char* GetName();
	void HandleTimerEvent(int te_id);
	void ApplyConfig(uint8_t cfgPos, const KTypeData& data);
	void Reset();
private:
	uint8_t configPos;
	char name[MAX_NAME_LENGTH];
	float ctemp, ltemp /*log temp*/;
	uint32_t sec;//interval in millis
	uint32_t last_read;
	//AverageI avgi, avgHour;
	void SetTempReadInterval(int s);//seconds
	void SetName(const char* name);
	void readTemp();
	void HandleStatusChange(void* Sender, Status value);
	void OnTimeSlice();
	void PrintInfo();
	AverageF<6> average;
};

class CKTypes: public StaticList<MAX_KTYPE_SENSORS, MAX31855>
{
public:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;

	MAX31855* GetByName(const char* name);
	void InitHW();
	void Begin();
	void Reset();
	void LoadConfig();
	bool GetConfigData(uint8_t cfgPos, KTypeData & data);
	bool ValidateSetupDataSet(JsonObject& jo);
	void ParseJson(JsonObject& jo);
};

extern CKTypes KTypes;

#endif
