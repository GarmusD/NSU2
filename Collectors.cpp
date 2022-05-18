#include "Collectors.h"

const int CCollectors::CURRENT_CONFIG_VERSION = 1;
const VersionInfo CCollectors::VINFO = { 'C', 'L', 'T', CURRENT_CONFIG_VERSION };
const char* CCollectors::CFG_FILE = "colltrs.cfg";

Collector::Collector(void)
{
	pump = NULL;
	Init();
}

void Collector::Init()
{
	pump_enabled = false;
	channels_closed = false;
	name[0] = 0;
	count = 0;
	max_count = 0;
	for (uint8_t i = 0; i < MAX_COLLECTOR_VALVES; i++)
	{
		valve_status[i] = false;
		valves[i].type = NC;
		valves[i].relay_channel = 0xFF;
	}
	pump = NULL;
	Events::Reset();
}


Collector::~Collector(void)
{
}

uint8_t Collector::GetConfigPos()
{
	return configPos;
}

void Collector::ApplyConfig(uint8_t cfgPos, const CollectorData & data)
{
	configPos = cfgPos;
	SetName(data.Name);
	SetValveCount(data.valve_count);
	for (uint8_t j = 0; j < data.valve_count; j++)
	{
		SetThermoValve(j, data.valves[j]);
	}
	SetCirculationPump(CirculationPumps.GetByName(data.CircPump));
}

void Collector::Reset()
{
	Init();
}

void Collector::SetValveCount(uint8_t value)
{
	if (value > MAX_COLLECTOR_VALVES)
	{
		char s[128];
		sprintf(s, "COLLECTOR: %s SetValveCount(). Value out of range: %d", name, value);
		Log.error(s);
		return;
	}
	max_count = value;
}

void Collector::SetThermoValve(uint8_t idx, const ThermoValve& valve)
{
	if(idx < 0 || idx >= max_count) return;
	valves[idx] = valve;
	if (valve.relay_channel != 0xFF)
	{
		count++;
	}
}

void Collector::OpenValve(uint8_t valve)
{
	if(valve <= 0 || valve > max_count)
	{
		char s[128];
		sprintf(s, "Collector '%s' OpenValve valve not in range - value: %d", name, valve);
		Log.error(s);
		return;
	}
	ThermoValve& v = valves[valve - 1];
	if (v.relay_channel != 0xFF)
	{
		if (valve_status[valve - 1]) return;

		valve_status[valve - 1] = true;
		PrintStatus();
		if (!pump_enabled) channels_closed = true;
		switch (v.type)
		{
		case NO:
			if (pump_enabled)
				RelayModules.CloseChannel(v.relay_channel);
			break;
		case NC:
			if (pump_enabled)
				RelayModules.OpenChannel(v.relay_channel);
			break;
		default:
			break;
		}
		if (pump) pump->ValveOpened();
	}
}

void Collector::CloseValve(uint8_t valve)
{
	if(valve <= 0 || valve > max_count)
	{
		char s[128];
		sprintf(s, "Collector '%s' CloseValve valve not in range - value: %d", name, valve);
		Log.error(s);
		return;
	}
	ThermoValve& v = valves[valve - 1];
	if (v.relay_channel != 0xFF)
	{
		if (!valve_status[valve - 1]) return;

		valve_status[valve - 1] = false;
		PrintStatus();
		switch (valves[valve - 1].type)
		{
		case NO:
			RelayModules.OpenChannel(valves[valve - 1].relay_channel);
			break;
		case NC:
			RelayModules.CloseChannel(valves[valve - 1].relay_channel);
			break;
		default:
			break;
		}
		if (pump) pump->ValveClosed();
	}
}

void Collector::CloseRelayChannels()
{
	char s[128];
	sprintf(s, "Collector '%s' CloseRelayChannels(). max_count: %d", name, max_count);
	Log.info(s);
	for(uint8_t i=0; i < max_count; i++)
	{
		//RelayModules.CloseChannel(valves[i].relay_channel);
		if (valve_status[i])
		{
			switch (valves[i].type)
			{
			case NO:
				RelayModules.OpenChannel(valves[i].relay_channel);
				break;
			case NC:
				RelayModules.CloseChannel(valves[i].relay_channel);
				break;
			default:
				break;
			}
		}
	}
	channels_closed = true;
}

void Collector::OpenRelayChannels()
{
	char s[128];
	sprintf(s, "COLLECTOR: '%s' OpenRelayChannels(). max_count: %d", name, max_count);
	Log.info(s);
	for(uint8_t i=0; i < max_count; i++)
	{
		if(valve_status[i])
		{
			switch (valves[i].type)
			{
				case NO:
					RelayModules.CloseChannel(valves[i].relay_channel);
					break;
				case NC:
					RelayModules.OpenChannel(valves[i].relay_channel);
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
	if(pump && count != 0)
	{
		pump->RegisterValveCount(count);
		pump->AddStatusChangeHandler(this);
		HandleStatusChange(pump, pump->getStatus());
	}
	if(!pump)
	{
		char s[128];
		sprintf(s, "For Collector '%s' setting CircPump NULL!", name);
		Log.error(s);
	}
}

void Collector::HandleStatusChange(void* Sender, Status status)
{
	char s[128];
	sprintf(s, "Collector '%s' StatusChange %s", name, Events::GetStatusName(status));
	Log.info(s);
	if(status == STATUS_ON || status == STATUS_MANUAL)
	{
		pump_enabled = true;
		if(channels_closed) OpenRelayChannels();
	}
	else if(status == STATUS_DISABLED)
	{
		pump_enabled = false;
		if(!channels_closed) CloseRelayChannels();
	}
	else if(status == STATUS_OFF)
	{
		pump_enabled = true;
	}
}

uint8_t Collector::GetValveCount()
{
	return max_count;
}

bool Collector::GetValveStatus(uint8_t pos)
{
	if (pos >= 0 && pos < MAX_COLLECTOR_VALVES)
		return valve_status[pos];
	return false;
}

void Collector::PrintStatus()
{
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
	root[jKeyTarget] = jTargetCollector;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	JsonArray arr = root.createNestedArray(jKeyStatus);
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

Collector * CCollectors::GetByConfigPos(uint8_t cfgPos)
{
	Collector* col;
	for (int i = 0; i < Count(); i++)
	{
		col = Get(i);
		if (col->GetConfigPos() == cfgPos) return col;
	}
	return nullptr;
}

void CCollectors::Reset()
{
	Clear();
}

void CCollectors::LoadConfig()
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_COLLECTORS; i++)
		{
			CollectorData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					Collector* col = Add();
					if (col)
					{
						col->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[128];
				sprintf(s, "Collectors config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_COLLECTORS; i++)
		{
			const CollectorData& data = Defaults.GetDefaultCollectorData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					Collector* col = Add();
					if (col)
					{
						col->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				char s[128];
				sprintf(s, "Collectors config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_COLLECTORS; i++)
		{
			const CollectorData& data = Defaults.GetDefaultCollectorData(i);
			if (data.Valid)
			{
				Collector* col = Add();
				if (col)
				{
					col->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool CCollectors::GetConfigData(uint8_t cfgPos, CollectorData & data)
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

bool CCollectors::ValidateSetupDataSet(JsonObject & jo)
{
	return jo.containsKey(jKeyConfigPos) &&
		jo.containsKey(jKeyEnabled) && 
		jo.containsKey(jKeyName) && 
		jo.containsKey(jCollectorCPumpName) && 
		jo.containsKey(jCollectorValveCount) && 
		jo.containsKey(jCollectorValves);
}

void CCollectors::ParseJson(JsonObject & jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char * action = jo[jKeyAction];
		if (strcmp(action, jKeySetup) == 0)
		{
			if (ValidateSetupDataSet(jo))
			{
				CollectorData data;
				data.Valid = jo[jKeyEnabled];
				Utilities::ClearAndCopyString(jo[jKeyName], data.Name);
				Utilities::ClearAndCopyString(jo[jCollectorCPumpName], data.CircPump);
				data.valve_count = jo[jCollectorValveCount];
				JsonArray ja = jo[jCollectorValves];
				for (uint8_t i = 0; i < MAX_COLLECTOR_VALVES; i++)
				{
					JsonObject joo = ja[i];
					data.valves[i].type = (ValveType)((uint8_t)joo[jCollectorValveType]);
					data.valves[i].relay_channel = joo[jCollectorValveChannel];
				}
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
							PrintJson.PrintResultOk(jTargetCollector, action, true);
							return;
						}
					}
					PrintJson.PrintResultError(jTargetCollector, jErrorFileWriteError);
					return;
				}
				PrintJson.PrintResultError(jTargetCollector, jErrorFileOpenError);
				return;
			}
			else
			{
				PrintJson.PrintResultError(jTargetCollector, jErrorInvalidDataSet);
			}
		}
		else
		{
			PrintJson.PrintResultUnknownAction(jTargetCollector, action);
		}
	}
	else
	{
		PrintJson.PrintResultFormatError();
	}
}

CCollectors Collectors;