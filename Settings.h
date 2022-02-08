#ifndef Settings_h
#define Settings_h

#include <SdFat.h>
#include <DallasTemperature.h>
#include "Logger.h"
//#include "EEPROM.h"
//#include "TSensors.h"
//#include "CirculationPump.h"
//#include "RelayModule.h"
//#include "ThermoValve.h"
//#include "Collector.h"
//#include "MAX31855.h"
//#include "ComfortZones.h"
//#include "switches.h"
//#include "WoodBoiler.h"
//#include "WaterBoiler.h"

#include "consts.h"

#include "SettingsOLD.h"

struct VersionInfo
{
	char vstring[3];
	int version;
};

struct DataAddresses
{
	unsigned long int MyAddr;
	unsigned long int SwitchAddr;
	unsigned long int TSensorsAddr;
	unsigned long int RelayAddr;
	unsigned long int TempTriggersAddr;
	unsigned long int CirculationPumpsAddr;
	unsigned long int CollectorsAddr;
	unsigned long int ComfortZonesAddr;
	unsigned long int KTypeAddr;
	unsigned long int BoilerisAddr;
	unsigned long int KatilasAddr;
};

///////////////////////////////////////////////////////////////////////////////////

struct TSensorsData
{
	byte Valid;
	SensorID sid;
	int interval;
};

struct SwitchData
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	char DependOn[MAX_NAME_LENGTH];
	byte depstate;
	byte forcestate;
};

struct RelayModuleData
{
	byte Valid;
	byte ActiveLow;
	byte Inverted;
};

struct TempTriggerPieceData
{
	byte Valid;
	char SensorName[MAX_NAME_LENGTH];
	byte Condition;
	int Temperature;
	int Histeresis;
};

struct TempTriggerData
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	TempTriggerPieceData ttpiece[4];
};

struct CircPumpData
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	char TempTriggerName[MAX_NAME_LENGTH];
	//byte Channel; Removed!!!!
	byte MaxSpeed;
	byte Spd1Channel;
	byte Spd2Channel;
	byte Spd3Channel;
};

struct CollectorsData
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	char CircPump[MAX_NAME_LENGTH];
	byte valve_count;
	ThermoValve valves[MAX_COLLECTOR_VALVES];
};

struct ComfortZonesData
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	char Title[MAX_NAME_LENGTH];
	char room_sensor[MAX_NAME_LENGTH];
	char floor_sensor[MAX_NAME_LENGTH];
	char col_name[MAX_NAME_LENGTH];
	int room_temp_hi;
	int room_temp_low; //
	int floor_temp_hi; //
	int floor_temp_low; //
	int histeresis;
	byte channel;
	byte low_temp_mode;
};

struct KTypeData
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	int interval;
};

struct WaterBoilerData
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	char TSensorName[MAX_NAME_LENGTH];
	char TempTriggerName[MAX_NAME_LENGTH];
	char CircPumpName[MAX_NAME_LENGTH];
	byte ElHeatingEnabled;
	byte ElHeatingChannel;
	ElHeatingData HeatingData[7];
};

struct WoodBoilerData
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	char TSensorName[MAX_NAME_LENGTH];
	char KTypeName[MAX_NAME_LENGTH];
	byte LadomatChannel;
	byte ExhaustFanChannel;
	int WorkingTemp;
	int WorkingHisteresis;
	int LadomatTemp; //<- nauja
	char LadomatTempTriggerName[MAX_NAME_LENGTH]; // <- nauja
	char WaterBoilerName[MAX_NAME_LENGTH]; // <- nauja
};

///////////////////////////////////////////////////////////////////////////////////
class SettingsClass
{
	
public:
	SettingsClass(void);
	~SettingsClass(void);

	bool Begin();
	bool BeginLongRead();
	void EndLongRead();

	const SwitchData& getSwitchData(byte pos);
	const TSensorsData& getTempSensorData(byte pos);
	const RelayModuleData& getRelayModuleData(byte pos);
	const TempTriggerData& getTempTriggerData(byte pos);
	const CircPumpData& getCircPumpData(byte pos);
	const CollectorsData& getCollectorsData(byte pos);
	const ComfortZonesData& getComfortZonesData(byte pos);
	const KTypeData& getKTypeData(byte pos);
	const WaterBoilerData& getWaterBoilerData(byte pos);
	const WoodBoilerData& getWoodBoilerData(byte pos);

	void setSwitchData(byte pos, const SwitchData& data);
	void setTempSensorData(byte pos, const TSensorsData& data);
	void setRelayModuleData(byte pos, const RelayModuleData& data);
	void setTempTriggerData(byte pos, const TempTriggerData& data);
	void setCircPumpData(byte pos, const CircPumpData& data);
	void setCollectorsData(byte pos, const CollectorsData& data);
	void setComfortZonesData(byte pos, const ComfortZonesData& data);
	void setKTypeData(byte pos, const KTypeData& data);
	void setWoodBoilerData(byte pos, const WoodBoilerData& data);
	void setWaterBoilerData(byte pos, const WaterBoilerData& data);

	void clearSwitchData(byte pos);
	void clearTempSensorData(byte pos);
	void clearRelayModuleData(byte pos);
	void clearTempTriggerData(byte pos);
	void clearCircPumpData(byte pos);
	void clearCollectorsData(byte pos);
	void clearComfortZonesData(byte pos);
	void clearKTypeData(byte pos);
	void clearWoodBoilerData(byte pos);
	void clearWaterBoilerData(byte pos);

	bool IsReady();

private:
	bool longread;
	bool isReady;

	TSensorsData tsensorData;
	SwitchData switchData;
	RelayModuleData relayData;
	TempTriggerData triggerData;
	CircPumpData cpumpData;
	CollectorsData collectorData;
	ComfortZonesData czonesData;
	KTypeData ktypeData;
	WaterBoilerData waterBoilerData;
	WoodBoilerData woodBoilerData;

	SdFile file;
	DataAddresses addrtable;

	bool beginflashread(unsigned int addr);
	bool beginflashwrite(unsigned int addr);
	void endflash();

	byte readbyte();
	int readint();
	void writebyte(byte data);
	void writeint(int32_t data);
	void writezeros(int count);
	bool checkReadWrite(int value);
	bool checkVersion();
	void create_structure();

	//void convert(SdFile& oldFile, SdFile& newFile);
	void convert();

	const SwitchData& GetDefaultSwitchData(byte pos);
	const TSensorsData& GetDefaultTSensorData(byte pos);
	const RelayModuleData& GetDefaultRelayModuleData(byte pos);
	const TempTriggerData& GetDefaultTempTriggerData(byte pos);
	const CircPumpData& GetDefaultCircPumpData(byte pos);
	const CollectorsData& GetDefaultCollectorData(byte pos);
	const ComfortZonesData& GetDefaultComfortZoneData(byte pos);
	const KTypeData& GetDefaultKTypeData(byte pos);
	const WaterBoilerData& GetDefaultWaterBoilerData(byte pos);
	const WoodBoilerData& GetDefaultWoodBoilerData(byte pos);

	void Log_SD_Or_Pos_Error(byte pos);
};


extern SettingsClass Settings; 

#endif
