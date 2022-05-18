#ifndef TSensors_h
#define TSensors_h

#include "TSensorsDefs.h"
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
#include "Defaults.h"
#include "StaticList.h"
#include "FileManager.h"

enum TempReadStatus
{
	TR_WAIT,
	TR_DONE,
	TR_ERROR
};

//SensorID InvalidSensorID = { {0,0,0,0,0,0,0,0}, "" };

class TempSensor:public Events{
public:
	static int GetSensorNameAddr(SensorID* sid, char* buff, uint32_t buff_size);
	static bool StringToAddr(const char* saddr, uint8_t* addr);
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
	void SetResolution(uint8_t res);
	int getSensorAddr(char * buff, int size);	
	unsigned int getErrorCount();

	void ApplyConfig(uint8_t cfgPos, const TSensorData& data);	
	void Reset();
	void Simulate(float value);
private:
	uint8_t configPos;
	bool enabled;
	TempReadStatus status;
	float pround(float x);
	DallasTemperature* dt;
	uint32_t lastread;
	uint32_t lastdispatch;
	float currtemp, avgtemp;
	unsigned int interval, readinterval, readcount, errors;

	SensorID sid;
	AverageF<5> average;
};

class CTempSensors:public Events
{
public:
	CTempSensors(uint8_t wire, uint8_t testwire); //testwire - for sensor addr scan
	TempSensor* getByAddr(const uint8_t* addr);
	TempSensor* getByName(const char* name);
	TempSensor* getByIndex(int idx);
	void Begin();
	bool ScanSensors();
	uint8_t getSensorCount();
	void InitialReadAllTemperatures();
	void SensorFounded(uint8_t* da);
	void SetResolution(uint8_t res);
	void LoadConfig(void);
	bool GetConfigData(uint8_t cfgPos, TSensorData& data);
	void ParseJSON(JsonObject & jo);
	void Reset();
private:
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;
	
	uint8_t idx;//sensors index
	uint8_t cidx;//sensor index for temp reading
	uint8_t count;
	uint8_t resolution;
	//OneWire* onewire;
	OneWire onewire;
	//DallasTemperature* dallas;
	DallasTemperature dallas;
	StaticList<MAX_SENSOR_COUNT, TempSensor> sensors;
	bool ValidateSetupDataSet(ArduinoJson::JsonObject & jo);
	void OnTimeSlice();
	void HandleTemperatureChange(void* Sender, float t);
	bool initialread;
};

extern CTempSensors TSensors;

#endif


