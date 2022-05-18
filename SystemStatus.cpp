#include "SystemStatus.h"

#include "TSensors.h"
#include "switches.h"
#include "RelayModules.h"
#include "TempTriggers.h"
#include "CirculationPumps.h"
#include "Collectors.h"
#include "ComfortZones.h"
#include "WaterBoilers.h"
#include "WoodBoilers.h"
#include "SystemFans.h"
#include "Alarms.h"


CSystemStatus::CSystemStatus()
{
	Reset();
}

CSystemStatus::~CSystemStatus()
{
}

void CSystemStatus::begin()
{
	Timers.AddOneSecHandler(this, 1);
	SetState(SystemRunning);
}

void CSystemStatus::Reset()
{
	Events::Reset();
	upTime = 0;
	currState = SystemBooting;
	timeIsSet = false;
	rebootReqested = false;
	rebootRequired = false;
}



void CSystemStatus::SetState(ESystemStatus state)
{
	currState = state;
	PrintSystemState();
}

bool CSystemStatus::GetRebootReqested()
{
	return rebootReqested;
}

void CSystemStatus::SetRebootRequired()
{
	rebootRequired = true;
}

bool CSystemStatus::GetRebootRequired()
{
	return rebootRequired;
}

void CSystemStatus::HandleTimerEvent(int te_id)
{
	upTime++;
}

bool CSystemStatus::TimeIsSet()
{
	return timeIsSet;
}

void CSystemStatus::PrintSystemState()
{
	DynamicJsonDocument jbuff(512);
	JsonObject root = jbuff.to<JsonObject>();
	root[jKeyTarget] = jTargetSystem;
	root[jKeyAction] = jKeySystemActionSystemStatus;
	switch (currState)
	{
	case SystemBooting:
		root[jKeyValue] = jKeySysStatusBooting;
		break;
	case SystemReadyPauseBoot:
		root[jKeyValue] = jKeySysStatusReadyPauseBoot;
		break;
	case SystemBootPaused:
		root[jKeyValue] = jKeySysStatusBootPaused;
		break;
	case SystemRunning:
		root[jKeyValue] = jKeySysStatusRunning;
		break;
	default:
		break;
	}
	root[jKeySysStatusFreeMem] = FreeMem::GetFreeMem();
	if (currState == SystemRunning)
	{
		root[jKeySysStatusUpTime] = upTime;
	}
	root[jKeySysStatusRebootRequired] = rebootRequired;
	PrintJson.Print(root);
}

void CSystemStatus::Snapshot()
{
	//read config uid

	//send TSensor info
	for (uint8_t i = 0; i < MAX_SENSOR_COUNT; i++)
	{
		TempSensor* ts = TSensors.getByIndex(i);
		if (ts != NULL)
		{
			DynamicJsonDocument jBuff(512);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jKeyTargetTSensor;
			root[jKeyAction] = jKeySystemActionSnapshot;
			root[jKeyContent] = "system";
			char sa[32];
			ts->getSensorAddr(sa, 32);
			root[jKeyTSensorAddr] = sa;
			root["temp"] = ts->getTemp();
			root[jKeyTSensorErrorCount] = ts->getErrorCount();
			PrintJson.Print(root);
		}
	}

	for (uint8_t i = 0; i < MAX_SENSOR_COUNT; i++)
	{
		TSensorData data;
		if (!TSensors.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultTSensorData(i);
		}
		{
			DynamicJsonDocument jBuff(512);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jKeyTargetTSensor;
			root[jKeyAction] = jKeySystemActionSnapshot;
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			char sa[32];
			snprintf(sa, 32, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 
							data.sid.addr[0], data.sid.addr[1], data.sid.addr[2], data.sid.addr[3], data.sid.addr[4], data.sid.addr[5], data.sid.addr[6], data.sid.addr[7]);
			root[jKeyTSensorAddr] = sa;
			root[jKeyName] = data.sid.name;
			root[jKeyTSensorInterval] = data.interval;

			PrintJson.Print(root);
		}
	}

	//Switches
	for (uint8_t i = 0; i < MAX_SWITCHES_COUNT; i++)
	{
		SwitchData data;
		if (!Switches.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultSwitchData(i);
		}
		{
			DynamicJsonDocument jBuff(512);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetSwitch;
			root[jKeyAction] = jKeySystemActionSnapshot;
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jSwitchDependName] = data.DependOn;
			root[jSwitchDependState] = Events::GetStatusName((Status)data.depstate);
			root[jSwitchForceState] = Events::GetStatusName((Status)data.forcestate);

			Switch* sw = Switches.GetByConfigPos(i);
			if (sw)
			{
				root[jKeyContent] = jValueConfigPlus;
				root[jSwitchIsForced] = sw->IsForced();
				root[jSwitchCurrState] = Events::GetStatusName((Status)sw->GetStatus());
			}
			PrintJson.Print(root);
		}
	}

	//Relays
	for (uint8_t i = 0; i < MAX_RELAY_MODULES; i++)
	{
		RelayModuleData data;
		if (!RelayModules.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultRelayModuleData(i);
		}
		{
			DynamicJsonDocument jBuff(512);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetRelay;
			root[jKeyAction] = jKeySystemActionSnapshot;
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jRelayActiveLow] = data.ActiveLow;
			root[jRelayInverted] = data.Inverted;
			if (data.Valid)
			{
				root[jKeyContent] = jValueConfigPlus;
				root[jRelayFlags] = RelayModules.GetFlags(i);
			}
			PrintJson.Print(root);
		}
	}

	//TempTriggers
	for (uint8_t i = 0; i < MAX_TEMP_TRIGGERS; i++)
	{
		TempTriggerData data;
		if (!TempTriggers.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultTempTriggerData(i);
		}
		{
			DynamicJsonDocument jBuff(1024);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetTempTrigger;
			root[jKeyAction] = jKeySystemActionSnapshot;			
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;

			JsonArray ja = root.createNestedArray(jTriggerPieces);
			for (int i = 0; i < MAX_TEMP_TRIGGER_PIECES; i++)
			{
				JsonObject ttp = ja.createNestedObject();
				ttp[jKeyEnabled] = data.ttpiece[i].Valid;
				ttp[jTriggerSensorName] = data.ttpiece[i].SensorName;
				ttp[jTriggerCondition] = data.ttpiece[i].Condition;
				ttp[jTriggerTemperature] = data.ttpiece[i].Temperature / 100.0f;
				ttp[jTriggerHisteresis] = data.ttpiece[i].Histeresis / 100.0f;
			}

			TempTrigger* trg = TempTriggers.GetByConfigPos(i);
			if (trg)
			{
				root[jKeyContent] = jValueConfigPlus;
				root[jKeyStatus] = trg->GetStatusName(trg->GetStatus());
			}
			PrintJson.Print(root);
		}
	}

	//CircPumps
	for (uint8_t i = 0; i < MAX_CIRCULATION_PUMPS; i++)
	{
		CircPumpData data;
		if (!CirculationPumps.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultCircPumpData(i);
		}
		{
			DynamicJsonDocument jBuff(1024);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetCircPump;
			root[jKeyAction] = jKeySystemActionSnapshot;			
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			//root[jCircPumpChannel] = data.Channel;
			root[jCircPumpMaxSpeed] = data.MaxSpeed;
			root[jKeyName] = data.Name;
			root[jCircPumpSpeed1Ch] = data.Spd1Channel;
			root[jCircPumpSpeed2Ch] = data.Spd2Channel;
			root[jCircPumpSpeed3Ch] = data.Spd3Channel;
			root[jCircPumpTTriggerName] = data.TempTriggerName;

			CirculationPump* cpump = CirculationPumps.GetByConfigPos(i);
			if (cpump)
			{
				root[jKeyContent] = jValueConfigPlus;
				root[jKeyStatus] = cpump->GetStatusName(cpump->getStatus());
				root[jCircPumpCurrentSpeed] = cpump->GetCurrentSpeed();
				root[jCircPumpValvesOpened] = cpump->GetOpenedValveCount();
			}

			PrintJson.Print(root);
		}
	}

	//Collectors
	for (uint8_t i = 0; i < MAX_COLLECTORS; i++)
	{
		CollectorData data;
		if (!Collectors.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultCollectorData(i);
		}
		{
			DynamicJsonDocument jBuff(2048);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetCollector;
			root[jKeyAction] = jKeySystemActionSnapshot;			
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jCollectorCPumpName] = data.CircPump;
			root[jCollectorValveCount] = data.valve_count;

			Collector *col = Collectors.GetByConfigPos(i);
			JsonArray valves = root.createNestedArray(jCollectorValves);
			for (int j = 0; j < data.valve_count; j++)
			{
				JsonObject valve = valves.createNestedObject();
				valve[jCollectorValveType] = (uint8_t)data.valves[j].type;
				valve[jCollectorValveChannel] = data.valves[j].relay_channel;
				if (col)
				{
					root[jKeyContent] = jValueConfigPlus;
					valve[jKeyStatus] = col->GetValveStatus(j);
				}
			}
			PrintJson.Print(root);
		}
	}

	//ComfortZones
	for (uint8_t i = 0; i < MAX_COMFORT_ZONES; i++)
	{
		ComfortZoneData data;
		if (!ComfortZones.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultComfortZoneData(i);
		}
		{
			DynamicJsonDocument jBuff(1024);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetComfortZone;
			root[jKeyAction] = jKeySystemActionSnapshot;			
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jCZoneTitle] = data.Title;
			root[jCZoneCollectorName] = data.col_name;
			root[jCZoneActuator] = data.actuator;
			root[jCZoneHisterezis] = data.histeresis / 100.0f;
			root[jCZoneRoomSensorName] = data.room_sensor;
			root[jCZoneRoomTempHigh] = data.room_temp_hi / 100.0f;
			root[jCZoneRoomTempLow] = data.room_temp_low / 100.0f;
			root[jCZoneFloorSensorName] = data.floor_sensor;
			root[jCZoneFloorTempHigh] = data.floor_temp_hi / 100.0f;
			root[jCZoneFloorTempLow] = data.floor_temp_low / 100.0f;
			root[jCZoneLowTempMode] = data.low_temp_mode;

			ComfortZone* cz = ComfortZones.GetByConfigPos(i);
			if (cz)
			{
				root[jKeyContent] = jValueConfigPlus;
				root[jCZoneCurrRoomTemp] = cz->GetCurrRoomTemp();
				root[jCZoneCurrFloorTemp] = cz->GetCurrFloorTemp();
				root[jCZoneActuatorOpened] = cz->GetValveOpenedStatus();
			}
			PrintJson.Print(root);
		}
	}

	//KTypes
	for (uint8_t i = 0; i < MAX_KTYPE_SENSORS; i++)
	{
		KTypeData data;
		if (!KTypes.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultKTypeData(i);
		}
		{
			DynamicJsonDocument jBuff(512);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetKType;
			root[jKeyAction] = jKeySystemActionSnapshot;			
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jKTypeInterval] = data.interval;

			MAX31855* ktp = KTypes.GetByName(data.Name);
			if (ktp)
			{
				root[jKeyContent] = jValueConfigPlus;
				root[jKeyValue] = ktp->GetTemp();
			}
			PrintJson.Print(root);
		}
	}

	//WaterBoilers
	for (uint8_t i = 0; i < MAX_WATERBOILER_COUNT; i++)
	{
		WaterBoilerData data;
		if (!WaterBoilers.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultWaterBoilerData(i);
		}
		{
			DynamicJsonDocument jBuff(2048);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetWaterBoiler;
			root[jKeyAction] = jKeySystemActionSnapshot;			
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jWaterBoilerTempSensorName] = data.TSensorName;
			root[jWaterBoilerCPumpName] = data.CircPumpName;
			root[jWaterBoilerTTriggerName] = data.TempTriggerName;
			root[jWaterBoilerElHeatingEnabled] = data.ElHeatingEnabled;
			root[jWaterBoilerElPowerChannel] = data.ElHeatingChannel;

			JsonArray heatingarray = root.createNestedArray(jWaterBoilerElHeatingData);
			for (int j = 0; j < MAX_WATERBOILER_EL_HEATING_DATA_COUNT; j++)
			{
				JsonObject powerdata = heatingarray.createNestedObject();
				powerdata[jWaterBoilerEHStartHour] = data.HeatingData[j].StartHour;
				powerdata[jWaterBoilerEHStartMin] = data.HeatingData[j].StartMin;
				powerdata[jWaterBoilerEHStopHour] = data.HeatingData[j].EndHour;
				powerdata[jWaterBoilerEHStopMin] = data.HeatingData[j].EndMin;
			}

			PrintJson.Print(root);
		}
	}

	//WoodBoilers
	for (uint8_t i = 0; i < MAX_WOOD_BOILERS_COUNT; i++)
	{
		WoodBoilerData data;
		if (!WoodBoilers.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultWoodBoilerData(i);
		}
		{
			DynamicJsonDocument jBuff(1024);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetWoodBoiler;
			root[jKeyAction] = jKeySystemActionSnapshot;			
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jWoodBoilerWorkingTemp] = data.WorkingTemp / 100.0f;
			root[jWoodBoilerHisteresis] = data.WorkingHisteresis / 100.0f;
			root[jWoodBoilerTempSensorName] = data.TSensorName;
			root[jWoodBoilerKTypeName] = data.KTypeName;
			root[jWoodBoilerExhaustFanChannel] = data.ExhaustFanChannel;
			root[jWoodBoilerLadomatChannel] = data.LadomatChannel;
			root[jWoodBoilerLadomatTriggerName] = data.LadomatTempTriggerName;
			root[jWoodBoilerLadomatWorkingTemp] = data.LadomatTemp / 100.0f;
			root[jWoodBoilerWBName] = data.WaterBoilerName;

			CWoodBoiler* boiler = WoodBoilers.Get(i);
			if (boiler)
			{
				root[jKeyContent] = jValueConfigPlus;
				root[jWoodBoilerCurrentTemp] = boiler->GetCurrentTemp();
				root[jKeyStatus] = boiler->GetStatusName(boiler->GetKatilasStatus());
				root[jWoodBoilerLadomatStatus] = boiler->GetStatusName(boiler->GetLadomatStatus());
				root[jWoodBoilerSmokeFanStatus] = boiler->GetStatusName(boiler->GetSmokeFanStatus());
				root[jWoodBoilerTempStatus] = boiler->getTempStatusString();
			}
			PrintJson.Print(root);
		}
	}

	//SystemFans
	for (uint8_t i = 0; i < MAX_SYSTEM_FAN_COUNT; i++)
	{
		SystemFanData data;
		if (!SysFans.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultSystemFanData(i);
		}
		{
			DynamicJsonDocument jBuff(512);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetSystemFan;
			root[jKeyAction] = jKeySystemActionSnapshot;
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jSysFanTSensorName] = data.TSensorName;
			root[jSysFanMinTemp] = data.MinTemp / 100.0f;
			root[jSysFanMaxTemp] = data.MaxTemp / 100.0f;

			SystemFan* sysfan = SysFans.GetByConfigPos(i);
			if (sysfan)
			{
				root[jKeyContent] = jValueConfigPlus;
				root[jKeyValue] = sysfan->GetCurrentPWM();
			}
			PrintJson.Print(root);
		}
		
	}

	//Alarm
	for (uint8_t i = 0; i < MAX_ALARM_COUNT; i++)
	{
		AlarmData data;
		if (!Alarms.GetConfigData(i, data))
		{
			data = Defaults.GetDefaultAlarmData(i);
		}
		{
			DynamicJsonDocument jBuff(512);
			JsonObject root = jBuff.to<JsonObject>();
			root[jKeyTarget] = jTargetAlarm;
			root[jKeyAction] = jKeySystemActionSnapshot;
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jAlarmTemp] = data.alarm_temp / 100.0f;
			root[jAlarmHisteresis] = data.histeresis / 100.0f;

			JsonArray heatingarray = root.createNestedArray(jAlarmChannelData);
			for (int j = 0; j < MAX_ALARM_CHANNELS; j++)
			{
				JsonObject chdata = heatingarray.createNestedObject();
				chdata[jAlarmChannel] = data.channelInfo[j].channel;
				chdata[jAlarmOpen] = data.channelInfo[j].opened;
			}

			CAlarm* al = Alarms.GetByConfigPos(i);
			if (al)
			{
				root[jKeyContent] = jValueConfigPlus;
				root[jKeyValue] = al->IsAlarm();
			}
			PrintJson.Print(root);
		}
	}

	//Inform system about snapshot finish
	PrintJson.PrintResultDone(jTargetSystem, jKeySystemActionSnapshot);
}

void CSystemStatus::SetDateTime(JsonObject jo)
{
	uint8_t hour = jo[jKeySysStatusHour];
	uint8_t min = jo[jKeySysStatusMinute];
	uint8_t sec = jo[jKeySysStatusSecond];
	uint8_t year = jo[jKeySysStatusYear];
	uint8_t month = jo[jKeySysStatusMonth];
	uint8_t day = jo[jKeySysStatusDay];

	rtc.setTime(hour, min, sec, 24);
	rtc.setDate(year, month, day, 1);
	timeIsSet = true;
	char s[64];
	sprintf(s, "Set time is done. Week day is: %d", rtc.weekDay(rtc.getTimestamp()));
	Log.debug(s);
	PrintJson.PrintResultOk(jTargetSystem, jKeySysStatusActionSetTime);
}

void CSystemStatus::ParseJson(JsonObject &jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char * action = jo[jKeyAction];
		if (strcmp(action, jKeySystemActionPauseBoot) == 0)
		{
			BootChecker.PauseBootRequested();
			return;
		}
		if (strcmp(action, jKeySystemActionSystemStatus) == 0)
		{
			SystemStatus.PrintSystemState();
			return;
		}
		if (strcmp(action, jKeySystemActionSnapshot) == 0)
		{
			SystemStatus.Snapshot();
			return;
		}
		if (strcmp(action, jKeySysStatusActionSetTime) == 0)
		{
			SetDateTime(jo);
			return;
		}
		if (strcmp(action, jKeySystemActionReboot) == 0)
		{
			const char * val = jo[jKeyValue];
			if (strcmp(val, "soft") == 0)
				rebootReqested = true;
			else if (strcmp(val, "hard") == 0)
				while (1) {}
			else if (strcmp(val, "zero") == 0)
			{
				//asm volatile ("jmp 0");
			}
			return;
		}
	}
	else
	{
		PrintJson.PrintResultError(jTargetSystem, jErrorInvalidDataSet);
	}
}

CSystemStatus SystemStatus;