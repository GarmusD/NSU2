#ifndef ComfortZones_h
#define ComfortZones_h

#include "StaticList.h"
#include "Events.h"
#include "TSensors.h"
#include "Collector.h"
#include "consts.h"
#include "switches.h"
#include "Logger.h"
//#include "EEPROM.h"
/*********************************************
*********************************************/

const int MAX_COMFORT_ZONES = 32;

class ComfortZone : public Events
{
public:
	ComfortZone(void);
	~ComfortZone(void);
	void Begin();
	void SetName(const char* value);
	void SetTitle(const char* value);
	const char* GetName();
	const char* GetTitle();
	void SetCollector(Collector* coll, byte valveidx);
	void SetFloorSensor(TempSensor* fsensor);
	void SetFloorSensorEnabled(bool value);
	void SetRoomSensor(TempSensor* rsensor);
	void SetRoomSensorEnabled(bool value);
	void SetRequestedTemperature(float r_temp_hi, float r_temp_low, float f_temp_hi, float f_temp_low, float hister);
	float GetCurrRoomTemp();
	float GetCurrFloorTemp();
	bool GetValveOpenedStatus();
private:
	bool enabled;
	char name[MAX_NAME_LENGTH];
	char title[MAX_NAME_LENGTH];
	byte idx;
	TempSensor* floorsensor;
	TempSensor* roomsensor;
	Collector* collector;
	Switch* tempswitch;
	Switch* opmodeswitch;
	byte valve_index;
	bool valve_opened;
	bool room_sensor_enabled;
	bool floor_sensor_enabled;
	float c_temp_room, c_temp_floor;
	float req_temp_room, req_temp_floor, room_temp_hi, room_temp_low, floor_temp_hi, floor_temp_low;
	float histeresis;
	void HandleTemperatureChange(void* Sender, float value);
	void HandleStatusChange(void* Sender, Status value);
	void SetValveOpenedStatus(bool value);
	void UpdateCurrentRoomTemperature(float value);
	void UpdateCurrentFloorTemperature(float value);
};

/*********************************************
*********************************************/
class CComfortZones : public StaticList<MAX_COMFORT_ZONES, ComfortZone>
{
public:
	ComfortZone* GetByName(const char* name);
	//ComfortZone* Add();
	//ComfortZone* Get(int i);
//private:
	//ComfortZone items[MAX_COMFORT_ZONES];
};

/*
template<int s> struct Wow;
struct foo {
	int a, b;
};
Wow<sizeof(ComfortZone)> wow;
*/

extern CComfortZones ComfortZones;
#endif
//Program size: 143 996 bytes (used 27% of a 524 288 byte maximum) (5,44 secs)
