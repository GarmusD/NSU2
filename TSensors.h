#ifndef TSensors_h
#define TSensors_h

#include "nsu_pins.h"
#include "consts.h"
#include "Events.h"
#include "Logger.h"
#include <inttypes.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "TimeSlice.h"
#include "Utilities.h"
#include "ArduinoJson.h"
#include "JsonPrinter.h"

#define MAX_SENSOR_COUNT 64
#define MAX_SENSOR_NAME 32

enum TempReadStatus
{
	TR_WAIT,
	TR_DONE,
	TR_ERROR
};

struct SensorID{
	DeviceAddress addr;//unsigned int addr[8];
	char name[MAX_SENSOR_NAME];
};

//SensorID InvalidSensorID = { {0,0,0,0,0,0,0,0}, "" };

class TempSensor:public Events{
public:
	TempSensor(/*uint8_t* addr, DallasTemperature* dtemp*/);
	void SetAddress(const uint8_t* addr);
	void SetDallas(DallasTemperature* dtemp);
	void SetValid(bool value);
	bool GetValid();
	void SetInterval(uint32_t t);//in seconds
	uint32_t GetInterval();
	uint8_t* getAddr();
	const char* getName();
	void setName(const char* name);
	float getTemp();
	float getAverageTemp();
	TempReadStatus readTemp(uint32_t t, bool forceread=false);
	void reset();//resets time
	void SetResolution(byte res);
	String getSensorAddr();	
	static String GetSensorNameAddr(SensorID* sid);
private:
	bool enabled;
	TempReadStatus status;
	float pround(float x);
	DallasTemperature* dt;
	uint32_t lastread;
	uint32_t lastdispatch;
	float currtemp, avgtemp;
	unsigned int interval, readinterval, readcount;

	SensorID sid;
	String getSensorNameAddr();
	byte repeat_count;
	AverageF<5> average;
};

class CTempSensors:public Events
{
public:
	CTempSensors(byte wire, byte testwire); //testwire - for sensor addr scan
	TempSensor* getByAddr(uint8_t* addr);
	TempSensor* getByName(const char* name);
	TempSensor* getByIndex(int idx);
	void Begin();
	bool ScanSensors();
	byte getSensorCount();
	void InitialReadAllTemperatures();
	void SensorFounded(uint8_t* da);
	void SetResolution(byte res);
private:
	byte idx;//sensors index
	byte cidx;//sensor index for temp reading
	byte count;
	byte resolution;
	//OneWire* onewire;
	OneWire onewire;
	//DallasTemperature* dallas;
	DallasTemperature dallas;
	StaticList<MAX_SENSOR_COUNT, TempSensor> sensors;
	void OnTimeSlice();
	void HandleTemperatureChange(void* Sender, float t);
	bool initialread;
};

extern CTempSensors TSensors;

#endif


