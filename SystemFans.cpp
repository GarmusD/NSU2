#include "SystemFans.h"

const int SystemFans::CURRENT_CONFIG_VERSION = 1;
const VersionInfo SystemFans::VINFO = { 'S', 'F', 'N', CURRENT_CONFIG_VERSION };
const char* SystemFans::CFG_FILE = "sysfan.cfg";

SystemFan::SystemFan()
{
	ts = NULL;
	mint = 0xFF;
	maxt = 0xFF;
	currentPWM = 0;
	configPos = 0xFF;
}


SystemFan::~SystemFan()
{
}

void SystemFan::Reset()
{
	ts = NULL;
	Events::Reset();
}

void SystemFan::Begin()
{
	pinMode(8, OUTPUT);
}

uint8_t SystemFan::GetCurrentPWM()
{
	return uint8_t();
}

void SystemFan::SetName(const char * n)
{
	strlcpy(name, n, MAX_NAME_LENGTH);
}

char * SystemFan::GetName()
{
	return name;
}

uint8_t SystemFan::GetConfigPos()
{
	return configPos;
}

void SystemFan::ApplyConfig(uint8_t cfgPos, const SystemFanData & data)
{
	configPos = cfgPos;
	mint = data.MinTemp / 100.0f;
	maxt = data.MaxTemp / 100.0f;
	SetTempSensor(TSensors.getByName(data.TSensorName));
}

void SystemFan::SetTempSensor(TempSensor * sensor)
{
	if (ts)
	{
		ts->RemoveTempChangeHandler(this);
		ts = NULL;
	}
	if (sensor)
	{
		ts = sensor;
		ts->AddTempChangeHandler(this);
		HandleTemperatureChange(ts, ts->getTemp());
	}
}

void SystemFan::SetTemperatures(float min, float max)
{
	mint = min;
	maxt = max;
}

void SystemFan::HandleTemperatureChange(void * Sender, float value)
{
	uint8_t val = 0;
	if (value >= mint)
	{
		val = map(value, mint, maxt, 125, 255);
		if (currentPWM != val)
		{
			currentPWM = val;
			PrintStatus();
			char s[64];
			sprintf(s, "SYSFAN: SysFan PWM value: %d", val);
			Log.debug(s);
		}
	}
	analogWrite(8, val);	
}

void SystemFan::PrintStatus()
{
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
	root[jKeyTarget] = jTargetSystemFan;
	root[jKeyTarget] = name;
	root[jKeyAction] = jKeyStatus;
	root[jKeyValue] = currentPWM;
	PrintJson.Print(root);
}

void SystemFan::OnTimeSlice()
{
}

void SystemFan::ParseJSON(JsonObject & jo)
{
}



SystemFan * SystemFans::GetByName(const char * n)
{
	SystemFan* fan;
	for (int i = 0; i < Count(); i++)
	{
		fan = Get(i);
		if (strcmp(n, fan->GetName()) == 0) return fan;
	}
	return nullptr;
}

SystemFan * SystemFans::GetByConfigPos(int cfgPos)
{
	SystemFan* fan;
	for (int i = 0; i < Count(); i++)
	{
		fan = Get(i);
		if (fan->GetConfigPos() == cfgPos) return fan;
	}
	return nullptr;
}

void SystemFans::Begin()
{
	for (int i = 0; i < Count(); i++)
	{
		Get(i)->Reset();
	}
}

void SystemFans::Reset()
{
	for (int i = 0; i < Count(); i++)
	{
		Get(i)->Reset();
	}
	Clear();
}

void SystemFans::LoadConfig()
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_SYSTEM_FAN_COUNT; i++)
		{
			SystemFanData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					SystemFan* sf = Add();
					if (sf)
					{
						sf->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[64];
				sprintf(s, "SysFan config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_SYSTEM_FAN_COUNT; i++)
		{
			const SystemFanData& data = Defaults.GetDefaultSystemFanData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					SystemFan* sf = Add();
					if (sf)
					{
						sf->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				char s[64];
				sprintf(s, "SysFan config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_SYSTEM_FAN_COUNT; i++)
		{
			const SystemFanData& data = Defaults.GetDefaultSystemFanData(i);
			if (data.Valid)
			{
				SystemFan* sf = Add();
				if (sf)
				{
					sf->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool SystemFans::GetConfigData(uint8_t cfgPos, SystemFanData & data)
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

bool SystemFans::ValidateSetupDataSet(JsonObject &jo)
{
	return jo.containsKey(jKeyConfigPos) &&
		jo.containsKey(jKeyEnabled) && 
		jo.containsKey(jKeyName) && 
		jo.containsKey(jSysFanTSensorName) && 
		jo.containsKey(jSysFanMinTemp) && 
		jo.containsKey(jSysFanMaxTemp);

	uint8_t Valid;
	char Name[MAX_NAME_LENGTH];
	char TSensorName[MAX_NAME_LENGTH];
	int MinTemp;
	int MaxTemp;
}

void SystemFans::ParseJson(JsonObject & jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char * action = jo[jKeyAction];
		if (strcmp(action, jKeySetup) == 0)
		{
			SystemFanData data;
			data.Valid = jo[jKeyEnabled];
			Utilities::ClearAndCopyString(jo[jKeyName], data.Name);
			Utilities::ClearAndCopyString(jo[jSysFanTSensorName], data.TSensorName);
			data.MinTemp = (int)((float)jo[jSysFanMinTemp] * 100);
			data.MaxTemp = (int)((float)jo[jSysFanMaxTemp] * 100);

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
						PrintJson.PrintResultOk(jTargetSystemFan, action, true);
						return;
					}
				}
				PrintJson.PrintResultError(jTargetSystemFan, jErrorFileWriteError);
				return;
			}
			PrintJson.PrintResultError(jTargetSystemFan, jErrorFileOpenError);
			return;
		}
		else
		{
			PrintJson.PrintResultUnknownAction(jTargetSystemFan, action);
		}
	}
	else
	{
		PrintJson.PrintResultFormatError();		
	}
}

SystemFans SysFans;