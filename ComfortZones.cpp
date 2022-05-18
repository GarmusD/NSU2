#include "ComfortZones.h"

const int CComfortZones::CURRENT_CONFIG_VERSION = 2;
const VersionInfo CComfortZones::VINFO = { 'C', 'Z', 'N', CURRENT_CONFIG_VERSION };
const char* CComfortZones::CFG_FILE = "cmfzones.cfg";

/*********************************************
*********************************************/

/*********************************************
*********************************************/
ComfortZone::ComfortZone(void)
{
	Reset();
}

ComfortZone::~ComfortZone(void)
{
	Events::Reset();
}

void ComfortZone::Reset()
{
	Log.debug("ComfortZone::Reset() - Events::Reset();");
	Events::Reset();
	Log.debug("ComfortZone::Reset() - Events::Reset(); DONE");
	req_temp_room = 20.0;
	req_temp_floor = 20.0;
	c_temp_room = -127.0f;
	c_temp_floor = -127.0f;
	histeresis = 0.5;
	valve_index = 0xFF;
	collector = NULL;
	roomsensor = NULL;
	floorsensor = NULL;
	valve_opened = false;
	enabled = true;
	configPos = 0xFF;
	name[0] = 0;
	title[0] = 0;

	room_sensor_enabled = false;
	floor_sensor_enabled = false;
}

uint8_t ComfortZone::GetConfigPos()
{
	return configPos;
}

void ComfortZone::ApplyConfig(uint8_t cfgPos, const ComfortZoneData & data)
{
	configPos = cfgPos;
	SetName(data.Name);
	SetTitle(data.Title);
	SetCollector(Collectors.GetByName(data.col_name), data.actuator);
	SetRequestedTemperature(
		(float)data.room_temp_hi / 100.0f,
		(float)data.room_temp_low / 100.0f,
		(float)data.floor_temp_hi / 100.0f,
		(float)data.floor_temp_low / 100.0f,
		(float)data.histeresis / 100.0f,
		(bool)data.low_temp_mode);
	if (strlen(data.room_sensor))
	{
		SetRoomSensor(TSensors.getByName(data.room_sensor));
	}
	if (strlen(data.floor_sensor))
	{
		SetFloorSensor(TSensors.getByName(data.floor_sensor));
	}

	Begin();
}

void ComfortZone::UpdateSettings(const ComfortZoneData & data)
{
	SetRequestedTemperature(
		(float)data.room_temp_hi / 100.0f,
		(float)data.room_temp_low / 100.0f,
		(float)data.floor_temp_hi / 100.0f,
		(float)data.floor_temp_low / 100.0f,
		(float)data.histeresis / 100.0f,
		(bool)data.low_temp_mode);
}

void ComfortZone::Begin()
{
	tempswitch = Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_TEMPERATURE_MODE]);// NULL;
	tempswitch->AddStatusChangeHandler(this);

	opmodeswitch = Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_WINTER_MODE]);// NULL;
	opmodeswitch->AddStatusChangeHandler(this);

	HandleStatusChange(tempswitch, tempswitch->GetStatus());
	HandleStatusChange(opmodeswitch, opmodeswitch->GetStatus());
}

void ComfortZone::SetName(const char* value)
{
	strlcpy(this->name, value, MAX_NAME_LENGTH);
}

void ComfortZone::SetTitle(const char* value)
{
	strlcpy(this->title, value, MAX_NAME_LENGTH);
}

const char* ComfortZone::GetName()
{
	return name;
}

const char* ComfortZone::GetTitle()
{
	return title;
}

void ComfortZone::SetCollector(Collector* coll, uint8_t valveidx)
{
	collector = coll;
	valve_index = valveidx;
	char s[128];
	if(collector)
	{
		sprintf(s, "ComfortZone ('%s') SetCollector - Collector OK. Valve_index: %d", name, valve_index);
	}
	else
	{
		sprintf(s, "ComfortZone ('%s') SetCollector - Collector NULL. Valve_index: %d", name, valve_index);
	}
	Log.debug(s);
}

void ComfortZone::SetFloorSensor(TempSensor* fsensor)
{
	if(floorsensor)
	{
		floorsensor->RemoveTempChangeHandler(this);
		floor_sensor_enabled = false;
	}
	floorsensor = fsensor;
	if(floorsensor)
	{
		floor_sensor_enabled = true;
		floorsensor->AddTempChangeHandler(this);
		HandleTemperatureChange(floorsensor, floorsensor->getTemp());
	}
	else
	{
		Log.error("ComfortZones::SetFloorSensor(). Sensor IS NULL.");
	}
}

void ComfortZone::SetRoomSensor(TempSensor* rsensor)
{
	if(roomsensor)
	{
		roomsensor->RemoveTempChangeHandler(this);
		room_sensor_enabled = false;
	}
	roomsensor = rsensor;
	if(roomsensor)
	{
		room_sensor_enabled = true;
		roomsensor->AddTempChangeHandler(this);
		HandleTemperatureChange(roomsensor, roomsensor->getTemp());
	}
	else
	{
		Log.error("ComfortZones::SetRoomSensor(). Sensor IS NULL.");
	}
}

void ComfortZone::SetRequestedTemperature(float r_temp_hi, float r_temp_low, float f_temp_hi, float f_temp_low, float hister, bool ltempMode)
{
	lowTempMode = ltempMode;

	room_temp_hi = r_temp_hi;
	room_temp_low = r_temp_low;

	floor_temp_hi = f_temp_hi;
	floor_temp_low = f_temp_low;

	req_temp_room = lowTempMode ? r_temp_low : r_temp_hi;
	req_temp_floor = lowTempMode ? f_temp_low : f_temp_hi ;
	
	histeresis = abs(hister);

	if(tempswitch)
	{
		HandleStatusChange(tempswitch, tempswitch->GetStatus());
	}
}

void ComfortZone::HandleTemperatureChange(void* Sender, float value)
{
	char s[128];
	sprintf(s, "ComfortZone::HandleTemperatureChange(). Start. Value: %.1f. Enabled=%s", value, enabled?"true":"false");
	Log.debug(s);
	if(!enabled) return;
	if (Sender == roomsensor)
	{
		Log.debug("ComfortZone::HandleTemperatureChange(). Calling UpdateCurrentRoomTemperature()");
		UpdateCurrentRoomTemperature(value);
	}
	else
	{
		sprintf(s, "ComfortZone::HandleTemperatureChange(). Sender != roomsensor. Sender=%ld, value: %.1f", (long)Sender, value);
		Log.debug(s);
	}
	if(Sender == floorsensor) UpdateCurrentFloorTemperature(value);

	if((roomsensor != NULL && room_sensor_enabled) && (floorsensor != NULL && floor_sensor_enabled))
	{
		Log.debug("Comfort zone Part1 - room and floor sensors enabled");
		if(c_temp_room >= req_temp_room && c_temp_floor >= req_temp_floor)
		{
			if(valve_index != 0xFF && collector != NULL && valve_opened)
			{
				sprintf(s, "ComfortZone (%s): valve closing. ReqRoomFloorTemp: %.1f, SensorTemp: %.1f", name, req_temp_room, c_temp_room);
				Log.debug(s);
				SetValveOpenedStatus(false);
				collector->CloseValve(valve_index);
			}
		}
		else if((c_temp_room <= req_temp_room - histeresis) || (c_temp_floor <= req_temp_floor - histeresis) )
		{
			if(valve_index != 0xFF && collector != NULL && !valve_opened)
			{
				sprintf(s, "ComfortZone (%s): valve opening. ReqRoomFloorTemp: %.1f, SensorTemp: %.1f", name, req_temp_room, c_temp_room);
				Log.debug(s);
				SetValveOpenedStatus(true);
				collector->OpenValve(valve_index);
			}
		}
	}
	else if(roomsensor != NULL && room_sensor_enabled)
	{
		Log.debug("Comfort zone Part2 - only room sensor enabled");
		if(c_temp_room >= req_temp_room){
			if(valve_index != 0xFF && collector != NULL && valve_opened)
			{
				sprintf(s, "ComfortZone (%s): valve closing. ReqRoomTemp: %.1f, SensorTemp: %.1f", name, req_temp_room, c_temp_room);
				Log.debug(s);
				SetValveOpenedStatus(false);
				collector->CloseValve(valve_index);
			}
		}
		else if(c_temp_room <= req_temp_room - histeresis)
		{
			if(valve_index != 0xFF && collector != NULL && !valve_opened)
			{
				sprintf(s, "ComfortZone (%s): valve opening. ReqRoomTemp: %.1f, SensorTemp: %.1f", name, req_temp_room, c_temp_room);
				Log.debug(s);
				SetValveOpenedStatus(true);
				collector->OpenValve(valve_index);
			}
		}
	}
	else if(floorsensor != NULL && floor_sensor_enabled)
	{
		Log.debug("Comfort zone Part3 - only floor sensor enabled");
		if(c_temp_floor >= req_temp_floor)
		{
			if(valve_index != 0xFF && collector != NULL && valve_opened)
			{
				sprintf(s, "ComfortZone (%s): valve closing. ReqFloorTemp: %.1f, SensorTemp: %.1f", name, req_temp_room, c_temp_floor);
				Log.debug(s);
				SetValveOpenedStatus(false);
				collector->CloseValve(valve_index);
			}
		}
		else if(c_temp_floor <= req_temp_floor - histeresis)
		{
			if(valve_index != 0xFF && collector != NULL && !valve_opened)
			{
				sprintf(s, "ComfortZone (%s): valve opening. ReqFloorTemp: %.1f, SensorTemp: %.1f", name, req_temp_room, c_temp_floor);
				Log.debug(s);
				SetValveOpenedStatus(true);
				collector->OpenValve(valve_index);
			}
		}
	}
}

void ComfortZone::HandleStatusChange(void* Sender, Status value)
{
	Log.debug("ComfortZone::HandleStatusChange(). Starting.");
	if(Sender == tempswitch)
	{
		Log.debug("ComfortZone::HandleStatusChange(tempswitch).");
		if(value == STATUS_ON || value == STATUS_DISABLED_ON)
		{
			req_temp_room = lowTempMode ? room_temp_low : room_temp_hi;
			req_temp_floor = lowTempMode ? floor_temp_low : floor_temp_hi;
		}
		else if(value == STATUS_OFF || value == STATUS_DISABLED_OFF)
		{
			req_temp_room = room_temp_low;
			req_temp_floor = floor_temp_low;
		}
		if(roomsensor != NULL) HandleTemperatureChange(roomsensor, c_temp_room);
		if(floorsensor != NULL) HandleTemperatureChange(floorsensor, c_temp_floor);
	}
	if(Sender == opmodeswitch)
	{
		enabled = (value == STATUS_ON) || (value == STATUS_DISABLED_ON);
		char s[128];
		sprintf(s, "ComfortZone::HandleStatusChange(opmodeswitch). Result: %s", enabled?"true":"false");
		Log.debug(s);
		if(enabled)
		{
			if(roomsensor && room_sensor_enabled)
			{
				HandleTemperatureChange(roomsensor, roomsensor->getTemp());
			}
			if(floorsensor && floor_sensor_enabled)
			{
				HandleTemperatureChange(floorsensor, floorsensor->getTemp());
			}
		}
		else
		{
			if(valve_opened && collector != NULL)
			{
				SetValveOpenedStatus(false);
				collector->CloseValve(valve_index);
			}
		}
	}
}

void ComfortZone::UpdateCurrentRoomTemperature(float value)
{
	c_temp_room = value;
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
	root[jKeyTarget] = jTargetComfortZone;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	root[jKeyContent] = jCZoneCurrRoomTemp;
	root[jKeyValue] = value;
	PrintJson.Print(root);
}

void ComfortZone::UpdateCurrentFloorTemperature(float value)
{
	c_temp_floor = value;
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
	root[jKeyTarget] = jTargetComfortZone;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	root[jKeyContent] = jCZoneCurrFloorTemp;
	root[jKeyValue] = value;
	PrintJson.Print(root);
}


float ComfortZone::GetCurrRoomTemp()
{
	return c_temp_room;
}

float ComfortZone::GetCurrFloorTemp()
{
	return c_temp_floor;
}

void ComfortZone::SetValveOpenedStatus(bool value)
{
	if (valve_index == 0xFF)
		return;
	if (valve_opened != value)
	{
		valve_opened = value;
		DynamicJsonDocument jBuff(256);
		JsonObject root = jBuff.to<JsonObject>();
		root[jKeyTarget] = jTargetComfortZone;
		root[jKeyName] = name;
		root[jKeyAction] = jValueInfo;
		root[jKeyContent] = jCZoneActuatorOpened;
		root[jKeyValue] = value;
		PrintJson.Print(root);
	}
}

bool ComfortZone::GetValveOpenedStatus()
{
	return valve_opened;
}

/*********************************************
*********************************************/

ComfortZone* CComfortZones::GetByName(const char* name)
{
	ComfortZone* zone;
	for(int i=0; i<MAX_COMFORT_ZONES /*Count()*/; i++)
	{
		zone = Get(i);
		if(strcmp(name, zone->GetName()) == 0) return zone;
	}
	return NULL;
}

ComfortZone * CComfortZones::GetByConfigPos(uint8_t cfgPos)
{
	ComfortZone* zone;
	for (int i = 0; i < MAX_COMFORT_ZONES /*Count()*/; i++)
	{
		zone = Get(i);
		if (zone->GetConfigPos() == cfgPos) return zone;
	}
	return nullptr;
}

void CComfortZones::Reset()
{
	Clear();
}

void CComfortZones::LoadConfig()
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_COMFORT_ZONES; i++)
		{
			ComfortZoneData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					ComfortZone* cz = Add();
					if (cz)
					{
						cz->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[128];
				sprintf(s, "ComfortZone config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_COMFORT_ZONES; i++)
		{
			const ComfortZoneData& data = Defaults.GetDefaultComfortZoneData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					ComfortZone* cz = Add();
					if (cz)
					{
						cz->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				char s[128];
				sprintf(s, "ComfortZone config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_COMFORT_ZONES; i++)
		{
			const ComfortZoneData& data = Defaults.GetDefaultComfortZoneData(i);
			if (data.Valid)
			{
				ComfortZone* cz = Add();
				if (cz)
				{
					cz->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool CComfortZones::GetConfigData(uint8_t cfgPos, ComfortZoneData & data)
{
	if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
	{
		uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
		if (FileManager.FileSeek(fpos))
		{
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				FileManager.FileClose();
				return true;
			}
		}
		FileManager.FileClose();
	}
	return false;
}

bool CComfortZones::ValidateSetupDataSet(JsonObject & jo)
{
	return jo.containsKey(jKeyConfigPos) &&
		jo.containsKey(jKeyEnabled) && 
		jo.containsKey(jKeyName) && 
		jo.containsKey(jCZoneTitle) && 
		jo.containsKey(jCZoneRoomSensorName) && 
		jo.containsKey(jCZoneFloorSensorName) && 
		jo.containsKey(jCZoneCollectorName) && 
		jo.containsKey(jCZoneRoomTempHigh) && 
		jo.containsKey(jCZoneRoomTempLow) && 
		jo.containsKey(jCZoneFloorTempHigh) && 
		jo.containsKey(jCZoneFloorTempLow) && 
		jo.containsKey(jCZoneHisterezis) && 
		jo.containsKey(jCZoneActuator) && 
		jo.containsKey(jCZoneLowTempMode);
}

bool CComfortZones::ValidateUpdateDataSet(JsonObject & jo)
{
	return jo.containsKey(jKeyName) &&
		(
		jo.containsKey(jCZoneRoomTempHigh) ||
		jo.containsKey(jCZoneRoomTempLow) ||
		jo.containsKey(jCZoneFloorTempHigh) ||
		jo.containsKey(jCZoneFloorTempLow) ||
		jo.containsKey(jCZoneLowTempMode)
		);
}

void CComfortZones::ParseJson(JsonObject & jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char * action = jo[jKeyAction];
		if (strcmp(action, jKeySetup) == 0)
		{
			if (ValidateSetupDataSet(jo))
			{
				ComfortZoneData data;
				data.Valid = jo[jKeyEnabled];
				Utilities::ClearAndCopyString(jo[jKeyName], data.Name);
				Utilities::ClearAndCopyString(jo[jCZoneTitle], data.Title);
				Utilities::ClearAndCopyString(jo[jCZoneRoomSensorName], data.room_sensor);
				Utilities::ClearAndCopyString(jo[jCZoneFloorSensorName], data.floor_sensor);
				Utilities::ClearAndCopyString(jo[jCZoneCollectorName], data.col_name);
				data.room_temp_hi = (int)((float)jo[jCZoneRoomTempHigh] * 100.0f);
				data.room_temp_low = (int)((float)jo[jCZoneRoomTempLow] * 100.0f);
				data.floor_temp_hi = (int)((float)jo[jCZoneFloorTempHigh] * 100.0f);
				data.floor_temp_low = (int)((float)jo[jCZoneFloorTempLow] * 100.0f);
				data.histeresis = (int)((float)jo[jCZoneHisterezis] * 100.0f);
				data.actuator = jo[jCZoneActuator];
				data.low_temp_mode = jo[jCZoneLowTempMode];

				uint8_t cfgPos = jo[jKeyConfigPos];
				uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
				if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
				{
					if (FileManager.FileSeek(fpos))
					{
						if (FileManager.FileWriteBuff(&data, sizeof(data)))
						{
							FileManager.FileClose();
							SystemStatus.SetRebootRequired();
							PrintJson.PrintResultOk(jTargetComfortZone, action, true);
							return;
						}
					}
					PrintJson.PrintResultError(jTargetComfortZone, jErrorFileWriteError);
					return;
				}
				PrintJson.PrintResultError(jTargetComfortZone, jErrorFileOpenError);
				return;
			}
			else
			{
				PrintJson.PrintResultError(jTargetComfortZone, jErrorInvalidDataSet);
			}
		}
		else if(strcmp(action, jKeyUpdate) == 0)
		{
			if (ComfortZone* cz = GetByName(jo[jKeyName]))
			{
				ComfortZoneData data;
				uint8_t cfgPos = cz->GetConfigPos();
				uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
				if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
				{
					if (FileManager.FileSeek(fpos))
					{
						if (FileManager.FileReadBuffer(&data, sizeof(data)))
						{
							if (jo.containsKey(jCZoneRoomTempHigh))
								data.room_temp_hi = (int)((float)jo[jCZoneRoomTempHigh] * 100);
							if (jo.containsKey(jCZoneRoomTempLow))
								data.room_temp_low = (int)((float)jo[jCZoneRoomTempLow] * 100);
							if (jo.containsKey(jCZoneFloorTempHigh))
								data.floor_temp_hi = (int)((float)jo[jCZoneFloorTempHigh] * 100);
							if (jo.containsKey(jCZoneFloorTempLow))
								data.floor_temp_low = (int)((float)jo[jCZoneFloorTempLow] * 100);
							if (jo.containsKey(jCZoneLowTempMode))
								data.low_temp_mode = (uint8_t)jo[jCZoneLowTempMode];
							cz->UpdateSettings(data);
							if(FileManager.FileSeek(fpos))
								if (FileManager.FileWriteBuff(&data, sizeof(data)))
								{
									FileManager.FileClose();
									PrintJson.PrintResultOk(jTargetComfortZone, jKeyUpdate);
									return;
								}
						}
					}
					FileManager.FileClose();
					PrintJson.PrintResultError(jTargetComfortZone, jErrorFileWriteError);
					return;
				}
				PrintJson.PrintResultError(jTargetComfortZone, jErrorFileOpenError);
			}
			else
			{
				PrintJson.PrintResultError(jTargetComfortZone, jErrorItemByNameNotFound);
			}
		}
		else
		{
			PrintJson.PrintResultUnknownAction(jTargetComfortZone, action);
		}
	}
	else
	{
		PrintJson.PrintResultFormatError();
	}
}

CComfortZones ComfortZones;