#ifndef ComfortZones_h
#define ComfortZones_h

#include "ComfortZonesDefs.h"
#include "StaticList.h"
#include "Events.h"
#include "TSensors.h"
#include "Collectors.h"
#include "consts.h"
#include "switches.h"
#include "Logger.h"
#include "Defaults.h"
#include "SystemStatus.h"
//#include "EEPROM.h"
/*********************************************
*********************************************/

class ComfortZone : public Events
{
public:
	ComfortZone(void);
	~ComfortZone(void);
	void Begin(void);	
	const char* GetName(void);
	const char* GetTitle(void);
	uint8_t GetConfigPos(void);
	float GetCurrRoomTemp(void);
	float GetCurrFloorTemp(void);
	bool GetValveOpenedStatus(void);
	void ApplyConfig(uint8_t cfgPos, const ComfortZoneData& data);
	void UpdateSettings(const ComfortZoneData& data);
	void Reset();
private:
	uint8_t configPos;
	bool enabled;
	char name[MAX_NAME_LENGTH];
	char title[MAX_NAME_LENGTH];	
	TempSensor* floorsensor;
	TempSensor* roomsensor;
	Collector* collector;
	Switch* tempswitch;
	Switch* opmodeswitch;
	uint8_t valve_index;
	bool valve_opened;
	bool room_sensor_enabled;
	bool floor_sensor_enabled;
	bool lowTempMode;
	float c_temp_room, c_temp_floor;
	float req_temp_room, req_temp_floor, room_temp_hi, room_temp_low, floor_temp_hi, floor_temp_low;
	float histeresis;
	void SetName(const char* value);
	void SetTitle(const char* value);
	void SetCollector(Collector* coll, uint8_t valveidx);
	void SetFloorSensor(TempSensor* fsensor);
	void SetRoomSensor(TempSensor* rsensor);
	void SetRequestedTemperature(float r_temp_hi, float r_temp_low, float f_temp_hi, float f_temp_low, float hister, bool ltempMode);
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
	static const int CURRENT_CONFIG_VERSION;
	static const VersionInfo VINFO;
	static const char* CFG_FILE;

	ComfortZone* GetByName(const char* name);
	ComfortZone* GetByConfigPos(uint8_t cfgPos);
	void Reset(void);
	void LoadConfig(void);
	bool GetConfigData(uint8_t cfgPos, ComfortZoneData & data);	
	void ParseJson(JsonObject& jo);
private:
	bool ValidateSetupDataSet(JsonObject& jo);
	bool ValidateUpdateDataSet(JsonObject& jo);
};


extern CComfortZones ComfortZones;
#endif
