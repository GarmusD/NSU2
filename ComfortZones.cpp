#include "ComfortZones.h"

/*********************************************
*********************************************/

/*********************************************
*********************************************/
ComfortZone::ComfortZone(void)
{
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
	idx = 0xFF;
	name[0] = 0;
	title[0] = 0;

	room_sensor_enabled = false;
	floor_sensor_enabled = false;	
}

ComfortZone::~ComfortZone(void)
{
	if(tempswitch != NULL)
	{
		tempswitch->RemoveStatusChangeHandler(this);
	}
	if(opmodeswitch != NULL)
	{
		opmodeswitch->RemoveStatusChangeHandler(this);
	}
	if(roomsensor != NULL) roomsensor->RemoveTempChangeHandler(this);
	if(floorsensor != NULL) floorsensor->RemoveTempChangeHandler(this);
}

void ComfortZone::Begin()
{
	tempswitch = Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_TEMPERATURE_MODE].c_str());// NULL;
	tempswitch->AddStatusChangeHandler(this);

	opmodeswitch = Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_WINTER_MODE].c_str());// NULL;
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

void ComfortZone::SetCollector(Collector* coll, byte valveidx)
{
	collector = coll;
	valve_index = valveidx;
	if(collector){
		Log.debug("ComfortZone ('"+String(name)+"') SetCollector - Collector OK. Valve_index: "+String(valve_index));
	}else{
		Log.debug("ComfortZone ('"+String(name)+"') SetCollector - Collector NULL. Valve_index: "+String(valve_index));
	}
}

void ComfortZone::SetFloorSensor(TempSensor* fsensor)
{
	if(floorsensor)
	{
		floorsensor->RemoveTempChangeHandler(this);
		floor_sensor_enabled = false;
	}
	floorsensor = fsensor;
	if(floorsensor){
		floorsensor->AddTempChangeHandler(this);
		HandleTemperatureChange(floorsensor, floorsensor->getTemp());
	}
}

void ComfortZone::SetFloorSensorEnabled(bool value)
{
	floor_sensor_enabled = value;
	if(floorsensor)
	{
		HandleTemperatureChange(floorsensor, floorsensor->getTemp());
	}
	Log.debug("Comfort zone '"+String(name)+"' floor sensor enabled is " + String(floor_sensor_enabled));//?"enabled":"disabled");
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
		roomsensor->AddTempChangeHandler(this);
		HandleTemperatureChange(roomsensor, roomsensor->getTemp());
	}
	else
	{
		Log.error("ComfortZones::SetRoomSensor(). Sensor IS NULL.");
	}
}

void ComfortZone::SetRoomSensorEnabled(bool value)
{
	room_sensor_enabled = value;
	if(roomsensor)
	{
		HandleTemperatureChange(roomsensor, roomsensor->getTemp());
	}
	Log.debug("Comfort zone '"+String(name)+"' room sensor enabled is " + String(room_sensor_enabled));//?"enabled":"disabled");
}
/*
void ComfortZone::SetIndex(byte value)
{
	idx = value;
	if(idx >= 0 && idx != 0xFF)
	{
		SetRoomSensorEnabled(EEPROM.GetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx]));
		SetFloorSensorEnabled(EEPROM.GetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx]));
	}
	Log.debug("Comfort zone '"+String(name)+"' room sensor is " + String(room_sensor_enabled));//?"enabled":"disabled");
	Log.debug("Comfort zone '"+String(name)+"' floor sensor is " + String(floor_sensor_enabled));//?"enabled":"disabled");
}

/*
void ComfortZone::SetTemperatureSwitch(Switch* value)
{
	if(tempswitch)
	{
		tempswitch->RemoveTempChangeHandler(this);
	}
	tempswitch = value;
	if(tempswitch != NULL)
	{
		tempswitch->AddStatusChangeHandler(this);
		HandleStatusChange(tempswitch, tempswitch->GetStatus());
	}
}

void ComfortZone::SetOpModeSwitch(Switch* value)
{
	if(opmodeswitch)
	{
		opmodeswitch->RemoveStatusChangeHandler(this);
	}
	opmodeswitch = value;
	if(opmodeswitch)
	{
		opmodeswitch->AddStatusChangeHandler(this);
		HandleStatusChange(opmodeswitch, opmodeswitch->GetStatus());
	}
}
*/

void ComfortZone::SetRequestedTemperature(float r_temp_hi, float r_temp_low, float f_temp_hi, float f_temp_low, float hister)
{
	room_temp_hi = r_temp_hi;
	room_temp_low = r_temp_low;

	floor_temp_hi = f_temp_hi;
	floor_temp_low = f_temp_low;

	req_temp_room = r_temp_hi;
	req_temp_floor = f_temp_hi;
	
	histeresis = abs(hister);

	if(tempswitch)
	{
		HandleStatusChange(tempswitch, tempswitch->GetStatus());
	}
}

void ComfortZone::HandleTemperatureChange(void* Sender, float value)
{
	Log.debug("ComfortZone::HandleTemperatureChange(). Start. Value: "+String(value)+". Enabled="+String(enabled));
	if(!enabled) return;
	if (Sender == roomsensor)
	{
		Log.debug("ComfortZone::HandleTemperatureChange(). Calling UpdateCurrentRoomTemperature()");
		UpdateCurrentRoomTemperature(value);
	}
	else
	{
		Log.debug("ComfortZone::HandleTemperatureChange(). Sender != roomsensor. Sender=" + String((long)Sender) + ", value: " + String(value));
	}
	if(Sender == floorsensor) UpdateCurrentFloorTemperature(value);

	if((roomsensor != NULL && room_sensor_enabled) && (floorsensor != NULL && floor_sensor_enabled))
	{
		Log.debug("Comfort zone Part1 - room and floor sensors enabled");
		if(c_temp_room >= req_temp_room && c_temp_floor >= req_temp_floor){
			if(valve_index != 0xFF && collector != NULL && valve_opened){
				Log.debug("ComfortZone ("+String(name)+"): valve closing. ReqRoomFloorTemp: "+String(req_temp_room)+", SensorTemp: "+String(c_temp_room));
				SetValveOpenedStatus(false);
				collector->CloseValve(valve_index);
			}
		}else if((c_temp_room <= req_temp_room - histeresis) || (c_temp_floor <= req_temp_floor - histeresis) ){
			if(valve_index != 0xFF && collector != NULL && !valve_opened){
				Log.debug("ComfortZone ("+String(name)+"): valve opening. ReqRoomFloorTemp: "+String(req_temp_room)+", SensorTemp: "+String(c_temp_room));
				SetValveOpenedStatus(true);
				collector->OpenValve(valve_index);
			}
		}
	}
	else if(roomsensor != NULL && room_sensor_enabled){
		Log.debug("Comfort zone Part2 - only room sensor enabled");
		if(c_temp_room >= req_temp_room){
			if(valve_index != 0xFF && collector != NULL && valve_opened){
				Log.debug("ComfortZone ("+String(name)+"): valve closing. ReqRoomTemp: "+String(req_temp_room)+", SensorTemp: "+String(c_temp_room));
				SetValveOpenedStatus(false);
				collector->CloseValve(valve_index);
			}
		}else if(c_temp_room <= req_temp_room - histeresis){
			if(valve_index != 0xFF && collector != NULL && !valve_opened){
				Log.debug("ComfortZone ("+String(name)+"): valve opening. ReqRoomTemp: "+String(req_temp_room)+", SensorTemp: "+String(c_temp_room));
				SetValveOpenedStatus(true);
				collector->OpenValve(valve_index);
			}
		}
	}
	else if(floorsensor != NULL && floor_sensor_enabled)
	{
		Log.debug("Comfort zone Part3 - only floor sensor enabled");
		if(c_temp_floor >= req_temp_floor){
			if(valve_index != 0xFF && collector != NULL && valve_opened){
				Log.debug("ComfortZone ("+String(name)+"): valve closing. ReqFloorTemp: "+String(req_temp_room)+", SensorTemp: "+String(c_temp_floor));
				SetValveOpenedStatus(false);
				collector->CloseValve(valve_index);
			}
		}else if(c_temp_floor <= req_temp_floor - histeresis){
			if(valve_index != 0xFF && collector != NULL && !valve_opened){
				Log.debug("ComfortZone ("+String(name)+"): valve opening. ReqFloorTemp: "+String(req_temp_room)+", SensorTemp: "+String(c_temp_floor));
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
			req_temp_room = room_temp_hi;
			req_temp_floor = floor_temp_hi;
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
		Log.debug("ComfortZone::HandleStatusChange(opmodeswitch). Result: "+String(enabled));
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
	StaticJsonBuffer<200> jBuff;
	JsonObject& root = jBuff.createObject();
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
	StaticJsonBuffer<200> jBuff;
	JsonObject& root = jBuff.createObject();
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
		StaticJsonBuffer<200> jBuff;
		JsonObject& root = jBuff.createObject();
		root[jKeyTarget] = jTargetComfortZone;
		root[jKeyName] = name;
		root[jKeyAction] = jValueInfo;
		root[jKeyContent] = jCZoneValveOpened;
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

/*
ComfortZone * CComfortZones::Add()
{
	return &items[0];
}

ComfortZone * CComfortZones::Get(int i)
{
	return &items[i];
}
*/


CComfortZones ComfortZones;