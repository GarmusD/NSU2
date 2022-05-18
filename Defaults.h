#ifndef Settings_h
#define Settings_h

#include "consts.h"
#include "Events.h"
#include "SwitchesDefs.h"
#include "TSensorsDefs.h"
#include "RelayModulesDefs.h"
#include "TempTriggersDefs.h"
#include "CirculationPumpsDefs.h"
#include "CollectorsDefs.h"
#include "ComfortZonesDefs.h"
#include "MAX31855Defs.h"
#include "WaterBoilersDefs.h"
#include "WoodBoilersDefs.h"
#include "SystemFansDefs.h"
#include "AlarmsDefs.h"

class DefaultsClass
{
public:
	const SwitchData& GetDefaultSwitchData(uint8_t pos);
	const TSensorData& GetDefaultTSensorData(uint8_t pos);
	const RelayModuleData& GetDefaultRelayModuleData(uint8_t pos);
	const TempTriggerData& GetDefaultTempTriggerData(uint8_t pos);
	const CircPumpData& GetDefaultCircPumpData(uint8_t pos);
	const CollectorData& GetDefaultCollectorData(uint8_t pos);
	const ComfortZoneData& GetDefaultComfortZoneData(uint8_t pos);
	const KTypeData& GetDefaultKTypeData(uint8_t pos);
	const WaterBoilerData& GetDefaultWaterBoilerData(uint8_t pos);
	const WoodBoilerData& GetDefaultWoodBoilerData(uint8_t pos);
	const SystemFanData& GetDefaultSystemFanData(uint8_t pos);
	const AlarmData& GetDefaultAlarmData(uint8_t pos);

private:
	TSensorData tsensorData;
	SwitchData switchData;
	RelayModuleData relayData;
	TempTriggerData triggerData;
	CircPumpData cpumpData;
	CollectorData collectorData;
	ComfortZoneData czonesData;
	KTypeData ktypeData;
	WaterBoilerData waterBoilerData;
	WoodBoilerData woodBoilerData;
	SystemFanData sysFanData;
	AlarmData alarmData;
};


extern DefaultsClass Defaults; 

#endif
