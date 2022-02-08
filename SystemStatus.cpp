#include "SystemStatus.h"
#include "BootChecker.h"
#include "WoodBoiler.h"

CSystemStatus::CSystemStatus()
{
	upTime = 0;
	currState = SystemBooting;
	timeIsSet = false;
}

CSystemStatus::~CSystemStatus()
{
}

void CSystemStatus::begin()
{
	Timers.AddOneSecHandler(this, 1);
	SetState(SystemRunning);
}

void CSystemStatus::SetState(ESystemStatus state)
{
	currState = state;
	PrintSystemState();
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
	StaticJsonBuffer<200> jbuff;
	JsonObject& root = jbuff.createObject();
	root[jKeyTarget] = jTargetSystem;
	root[jKeyAction] = jSystemActionSystemStatus;
	switch (currState)
	{
	case SystemBooting:
		root[jKeyValue] = jSysStatusBooting;
		break;
	case SystemReadyPauseBoot:
		root[jKeyValue] = jSysStatusReadyPauseBoot;
		break;
	case SystemBootPaused:
		root[jKeyValue] = jSysStatusBootPaused;
		break;
	case SystemRunning:
		root[jKeyValue] = jSysStatusRunning;
		break;
	default:
		break;
	}
	root[jKeySysStatusFreeMem] = FreeMem::GetFreeMem();
	if (currState == SystemRunning)
	{
		root[jKeySysStatusUpTime] = upTime;
	}
	PrintJson.Print(root);
}

void CSystemStatus::Snapshot()
{
	//read config uid

	//send TSensor info
	for (int i = 0; i < MAX_SENSOR_COUNT; i++)
	{
		TempSensor* ts = TSensors.getByIndex(i);
		if (ts != NULL)
		{
			StaticJsonBuffer<200> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetTSensor;
			root[jKeyAction] = jSystemActionSnapshot;
			root[jKeyContent] = "system";
			root[jKeyAddr] = ts->getSensorAddr();
			root["temp"] = ts->getTemp();
			PrintJson.Print(root);
		}
	}

	for (int i = 0; i < MAX_SENSOR_COUNT; i++)
	{
		const TSensorsData& data = Settings.getTempSensorData(i);
		{
			StaticJsonBuffer<512> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetTSensor;
			root[jKeyAction] = jSystemActionSnapshot;
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			String sa = String(data.sid.addr[0], HEX) + ":" + String(data.sid.addr[1], HEX) + ":" + String(data.sid.addr[2], HEX) + ":" + String(data.sid.addr[3], HEX) + ":" +
				String(data.sid.addr[4], HEX) + ":" + String(data.sid.addr[5], HEX) + ":" + String(data.sid.addr[6], HEX) + ":" + String(data.sid.addr[7], HEX);//TempSensor::AddrToString(data.sid.addr);
			root[jKeyAddr] = sa;
			root[jKeyName] = data.sid.name;
			root[jKeyInterval] = data.interval;

			PrintJson.Print(root);
		}
	}

	//Switches
	for (int i = 0; i < MAX_SWITCHES_COUNT; i++)
	{
		const SwitchData& data = Settings.getSwitchData(i);
		{
			StaticJsonBuffer<512> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetSwitch;
			root[jKeyAction] = jSystemActionSnapshot;
			root[jKeyContent] = jValueConfigPlus;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jSwitchDependName] = data.DependOn;
			root[jSwitchDependState] = Events::GetStatusName((Status)data.depstate);
			root[jSwitchForceState] = Events::GetStatusName((Status)data.forcestate);

			Switch* sw = Switches.Get(i);
			root[jSwitchIsForced] = sw->IsForced();
			root[jSwitchCurrState] = Events::GetStatusName((Status)sw->GetStatus());
			PrintJson.Print(root);
		}
	}

	//Relays
	for (int i = 0; i < MAX_RELAY_MODULES; i++)
	{
		const RelayModuleData& data = Settings.getRelayModuleData(i);
		{
			StaticJsonBuffer<512> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetRelay;
			root[jKeyAction] = jSystemActionSnapshot;
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jRelayActiveLow] = data.ActiveLow;
			root[jRelayInverted] = data.Inverted;
			root[jRelayFlags] = RelayModules.GetFlags(i);
			PrintJson.Print(root);
		}
	}

	//TempTriggers
	for (int i = 0; i < MAX_TEMP_TRIGGERS; i++)
	{
		const TempTriggerData& data = Settings.getTempTriggerData(i);
		{
			StaticJsonBuffer<1024> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetTempTrigger;
			root[jKeyAction] = jSystemActionSnapshot;			
			root[jKeyContent] = jValueConfigPlus;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;

			JsonArray &ja = root.createNestedArray(jTriggerPieces);
			for (int i = 0; i < MAX_TEMP_TRIGGER_PIECES; i++)
			{
				JsonObject & ttp = ja.createNestedObject();
				ttp[jKeyEnabled] = data.ttpiece[i].Valid;
				ttp[jTriggerSensorName] = data.ttpiece[i].SensorName;
				ttp[jTriggerCondition] = data.ttpiece[i].Condition;
				ttp[jTriggerTemperature] = data.ttpiece[i].Temperature / 100.0f;
				ttp[jTriggerHisteresis] = data.ttpiece[i].Histeresis / 100.0f;
			}

			TempTrigger* trg = TempTriggers.Get(i);
			root[jKeyStatus] = trg->GetStatusName(trg->GetStatus());
			PrintJson.Print(root);
		}
	}

	//CircPumps
	for (int i = 0; i < MAX_CIRCULATION_PUMPS; i++)
	{
		const CircPumpData& data = Settings.getCircPumpData(i);
		{
			StaticJsonBuffer<512> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetCircPump;
			root[jKeyAction] = jSystemActionSnapshot;			
			root[jKeyContent] = jValueConfigPlus;
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

			CirculationPump* cpump = CirculationPumps.Get(i);
			root[jKeyStatus] = cpump->GetStatusName(cpump->getStatus());
			root[jCircPumpCurrentSpeed] = cpump->GetCurrentSpeed();
			root[jCircPumpValvesOpened] = cpump->GetOpenedValveCount();

			PrintJson.Print(root);
		}
	}

	//Collectors
	for (int i = 0; i < MAX_COLLECTORS; i++)
	{
		const CollectorsData& data = Settings.getCollectorsData(i);
		{
			StaticJsonBuffer<2048> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetCollector;
			root[jKeyAction] = jSystemActionSnapshot;			
			root[jKeyContent] = jValueConfigPlus;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jCollectorCPumpName] = data.CircPump;
			root[jCollectorValveCount] = data.valve_count;
			Collector *col = Collectors.Get(i);
			JsonArray& valves = root.createNestedArray(jCollectorValves);
			for (int j = 0; j < data.valve_count; j++)
			{
				JsonObject& valve = valves.createNestedObject();// ("valve" + String(i));
				valve[jCollectorValveType] = (byte)data.valves[j].type;
				valve[jCollectorValveChannel] = data.valves[j].relay_channel;
				valve[jKeyStatus] = col->GetValveStatus(j);
			}
			PrintJson.Print(root);
		}
	}

	//ComfortZones
	for (int i = 0; i < MAX_COMFORT_ZONES; i++)
	{
		const ComfortZonesData& data = Settings.getComfortZonesData(i);
		{
			StaticJsonBuffer<768> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetComfortZone;
			root[jKeyAction] = jSystemActionSnapshot;			
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jCZoneTitle] = data.Title;
			root[jCZoneCollectorName] = data.col_name;
			root[jCZoneChannel] = data.channel;
			root[jCZoneHisterezis] = data.histeresis / 100.0f;
			root[jCZoneRoomSensorName] = data.room_sensor;
			root[jCZoneRoomTempHigh] = data.room_temp_hi / 100.0f;
			root[jCZoneRoomTempLow] = data.room_temp_low / 100.0f;
			root[jCZoneFloorSensorName] = data.floor_sensor;
			root[jCZoneFloorTempHigh] = data.floor_temp_hi / 100.0f;
			root[jCZoneFloorTempLow] = data.floor_temp_low / 100.0f;
			ComfortZone* cz = ComfortZones.Get(i);
			if (cz)
			{
				root[jKeyContent] = jValueConfigPlus;
				root[jCZoneCurrRoomTemp] = cz->GetCurrRoomTemp();
				root[jCZoneCurrFloorTemp] = cz->GetCurrFloorTemp();
				root[jCZoneValveOpened] = cz->GetValveOpenedStatus();
			}
			PrintJson.Print(root);
		}
	}

	//KTypes
	for (int i = 0; i < MAX_KTYPE_SENSORS; i++)
	{
		const KTypeData &data = Settings.getKTypeData(i);
		{
			StaticJsonBuffer<512> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetKType;
			root[jKeyAction] = jSystemActionSnapshot;			
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jKTypeInterval] = data.interval;
			MAX31855* ktp = KTypes.GetByName(data.Name);
			if (ktp)
			{
				root[jKeyValue] = ktp->GetTemp();
			}
			PrintJson.Print(root);
		}
	}

	//WaterBoilers
	for (int i = 0; i < MAX_WATERBOILER_COUNT; i++)
	{
		const WaterBoilerData& data = Settings.getWaterBoilerData(i);
		{
			StaticJsonBuffer<1024> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetWaterBoiler;
			root[jKeyAction] = jSystemActionSnapshot;			
			root[jKeyContent] = jValueConfig;
			root[jKeyConfigPos] = i;
			root[jKeyEnabled] = data.Valid;
			root[jKeyName] = data.Name;
			root[jWaterBoilerTempSensorName] = data.TSensorName;
			root[jWaterBoilerCPumpName] = data.CircPumpName;
			root[jWaterBoilerTTriggerName] = data.TempTriggerName;
			root[jWaterBoilerElHeatingEnabled] = data.ElHeatingEnabled;
			root[jWaterBoilerElPowerChannel] = data.ElHeatingChannel;

			JsonArray& heatingarray = root.createNestedArray(jWaterBoilerElHeatingData);
			for (int j = 0; j < MAX_WATERBOILER_EL_HEATING_DATA_COUNT; j++)
			{
				JsonObject& powerdata = heatingarray.createNestedObject();
				powerdata[jWaterBoilerEHStartHour] = data.HeatingData[j].StartHour;
				powerdata[jWaterBoilerEHStartMin] = data.HeatingData[j].StartMin;
				powerdata[jWaterBoilerEHStopHour] = data.HeatingData[j].EndHour;
				powerdata[jWaterBoilerEHStopMin] = data.HeatingData[j].EndMin;
			}

			PrintJson.Print(root);
		}
	}

	//WoodBoilers
	for (int i = 0; i < MAX_WOOD_BOILERS_COUNT; i++)
	{
		const WoodBoilerData& data = Settings.getWoodBoilerData(i);
		{
			StaticJsonBuffer<1024> jBuff;
			JsonObject& root = jBuff.createObject();
			root[jKeyTarget] = jTargetWoodBoiler;
			root[jKeyAction] = jSystemActionSnapshot;			
			root[jKeyContent] = jValueConfigPlus;
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
				root[jWoodBoilerCurrentTemp] = boiler->GetCurrentTemp();
				root[jKeyStatus] = boiler->GetStatusName(boiler->GetKatilasStatus());
				root[jWoodBoilerLadomatStatus] = boiler->GetStatusName(boiler->GetLadomatStatus());
				root[jWoodBoilerSmokeFanStatus] = boiler->GetStatusName(boiler->GetSmokeFanStatus());
				root[jWoodBoilerTempStatus] = boiler->getTempStatusString();
			}
			PrintJson.Print(root);
		}
	}

	//Inform system about snapshot finish
	PrintJson.PrintResultDone(jTargetSystem, jSystemActionSnapshot);
}

void CSystemStatus::SetDateTime(JsonObject& jo)
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
	Log.debug("Set time is done. Week day is: " + String(rtc.weekDay(rtc.getTimestamp())));
	PrintJson.PrintResultOk(jTargetSystem, jKeySysStatusActionSetTime);
}

void CSystemStatus::ParseJson(JsonObject &jo)
{
	if (jo.containsKey(jKeyAction))
	{
		String action = jo[jKeyAction];
		if (action.equals(jSystemActionPauseBoot))
		{
			BootChecker.PauseBootRequested();
			return;
		}
		if (action.equals(jSystemActionSystemStatus))
		{
			SystemStatus.PrintSystemState();
			return;
		}
		if (action.equals(jSystemActionSnapshot))
		{
			SystemStatus.Snapshot();
			return;
		}
		if (action.equals(jKeySysStatusActionSetTime))
		{
			SetDateTime(jo);
			return;
		}
	}
	else
	{
		PrintJson.PrintResultError(jTargetSystem, jErrorInvalidDataSet);
	}
}

CSystemStatus SystemStatus;