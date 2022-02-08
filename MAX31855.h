#ifndef MAX31855_h
#define MAX31855_h

#include "Logger.h"
#include "Events.h"
#include "StaticList.h"
#include "utilities.h"
#include "Timers.h"
#include "consts.h"
#include "TimeSlice.h"

const int MAX_KTYPE_SENSORS = 1;

class MAX31855:public Events
{
public:
	MAX31855(void);
	~MAX31855(void);
	void Init();
	void Begin();
	int32_t GetTemp();
	//void TimeEvent(uint32_t t);
	void SetTempReadInterval(int s);//seconds
	void SetName(const char* name);
	const char* GetName();
	void HandleTimerEvent(int te_id);
	String GetInfoString();
private:
	char name[MAX_NAME_LENGTH];
	float ctemp, ltemp /*log temp*/;
	uint32_t sec;//interval in millis
	uint32_t last_read;
	//AverageI avgi, avgHour;
	void readTemp();
	void HandleStatusChange(void* Sender, Status value);
	void OnTimeSlice();
	void PrintInfo();
	AverageF<6> average;
};

class CKTypes: public StaticList<MAX_KTYPE_SENSORS, MAX31855>
{
public:
	MAX31855* GetByName(const char* name);
};

extern CKTypes KTypes;

#endif
