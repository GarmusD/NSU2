#include "Collector.h"

Collector::Collector(void)
{
	pump = NULL;
	name[0] = 0;
	count = 0xFF;
	for(byte i=0; i < MAX_COLLECTOR_VALVES; i++)
	{
		valve_status[i] = false;
	}
	pump_enabled = false;
	channels_closed = false;
}


Collector::~Collector(void)
{
}

void Collector::SetValveCount(byte value)
{
	if (value > MAX_COLLECTOR_VALVES)
	{
		Log.error("COLLECTOR: " + String(name) + " SetValveCount(). Value out of range: " + String(value));
		return;
	}
	count = value;
	Log.debug("Collector::SetValveCount() is " + String(count));
	if(pump && count != 0xFF)
	{
		pump->RegisterValveCount(count);
	}
}

void Collector::SetThermoValve(byte idx, ThermoValve valve)
{
	if(idx < 0 || idx >= count) return;
	valves[idx] = valve;
}

void Collector::OpenValve(byte valve)
{
	if(valve <= 0 || valve > count)
	{
		Log.error("Collector '"+String(name)+"' OpenValve valve not in range - value: "+String(valve));
		return;
	}
	if(valve_status[valve-1])
	{
		Log.error("Collector '"+String(name)+"' OpenValve valve is opened. Valve: "+String(valve));
		return;
	}	
	ThermoValve v = valves[valve-1];	
	valve_status[valve-1] = true;
	PrintStatus();
	Log.info("COLLECTOR: " + String(name) + " OpenValve " + String(valve - 1));
	if(!pump_enabled) channels_closed = true;
	switch (v.type)
	{
	case NO:
		if(pump_enabled) 
			RelayModules.CloseChannel(v.relay_channel);
		break;
	case NC:
		if(pump_enabled) 
			RelayModules.OpenChannel(v.relay_channel);		
		break;
	default:
		break;
	}
	if (pump) pump->ValveOpened();
}

void Collector::CloseValve(byte valve)
{
	if(valve <= 0 || valve > count) return;
	if(!valve_status[valve-1]) return;	
	ThermoValve v = valves[valve-1];
	valve_status[valve-1] = false;
	PrintStatus();
	Log.info("COLLECTOR: " + String(name) + " CloseValve " + String(valve - 1));
	switch (v.type)
	{
	case NO:
		RelayModules.OpenChannel(v.relay_channel);
		break;
	case NC:
		RelayModules.CloseChannel(v.relay_channel);		
		break;
	default:
		break;
	}
	if (pump) pump->ValveClosed();
}

void Collector::CloseRelayChannels()
{
	Log.info("COLLECTOR: " + String(name) + " CloseRelayChannels(). count: " + String(count));
	for(byte i=0; i < count; i++)
	{
		ThermoValve v = valves[i];
		RelayModules.CloseChannel(v.relay_channel);
	}
	channels_closed = true;
}

void Collector::OpenRelayChannels()
{
	Log.info("COLLECTOR: " + String(name) + " OpenRelayChannels(). count: " + String(count));
	for(byte i=0; i < count; i++)
	{
		if(valve_status[i])
		{
			ThermoValve v = valves[i];
			switch (v.type)
			{
				case NO:
					RelayModules.CloseChannel(v.relay_channel);
					break;
				case NC:
					RelayModules.OpenChannel(v.relay_channel);
					break;
				default:
					break;
			}
		}
	}
	channels_closed = false;
}

void Collector::SetName(const char* name)
{
	strlcpy(this->name, name, MAX_NAME_LENGTH);
}

const char* Collector::GetName()
{
	return name;
}

void Collector::SetCirculationPump(CirculationPump* circpump)
{
	pump = circpump;
	if(pump && count != 0xFF){
		pump->RegisterValveCount(count);
		pump->AddStatusChangeHandler(this);
		HandleStatusChange(pump, pump->getStatus());
	}
	if(!pump)
	{
		Log.error("For Collector '"+String(name)+"' setting CircPump NULL!");
	}
}

void Collector::HandleStatusChange(void* Sender, Status status){
	Log.info("Collector " + String(name) + " StatusChange " +Events::GetStatusName(status));
	if(status == STATUS_ON || status == STATUS_MANUAL){
		pump_enabled = true;
		if(channels_closed) OpenRelayChannels();
	}else if(status == STATUS_DISABLED){
		pump_enabled = false;
		if(!channels_closed) CloseRelayChannels();
	}else if(status == STATUS_OFF){
		pump_enabled = true;
	}
}

String Collector::GetInfoString()
{
	String str = "INFO: COLLECTOR: "+String(name);
	for(int i=0; i < MAX_COLLECTOR_VALVES; i++)
	{
		str += " "+String(valve_status[i]);
	}
	return str;
}

byte Collector::GetValveCount()
{
	if (count == 0xFF) return 0;
	return count;
}

bool Collector::GetValveStatus(byte pos)
{
	if (pos >= 0 && pos < MAX_COLLECTOR_VALVES)
		return valve_status[pos];
	return false;
}

void Collector::PrintStatus()
{
	StaticJsonBuffer<512> jBuff;
	JsonObject& root = jBuff.createObject();
	root[jKeyTarget] = jTargetCollector;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	JsonArray& arr = root.createNestedArray(jKeyStatus);
	for (int j = 0; j < GetValveCount(); j++)
	{
		arr.add(GetValveStatus(j));
	}
	PrintJson.Print(root);
}

Collector* CCollectors::GetByName(const char* name)
{
	Collector* col;
	for(int i=0; i<Count(); i++)
	{
		col = Get(i);
		if(strcmp(name, col->GetName()) == 0) return col;
	}
	return NULL;
}

CCollectors Collectors;