#include "Alarms.h"

const int CAlarms::CURRENT_CONFIG_VERSION = 2;
const VersionInfo CAlarms::VINFO = { 'A', 'L', 'R', CURRENT_CONFIG_VERSION };
const char* CAlarms::CFG_FILE = "alarms.cfg";

CAlarm::CAlarm()
{
	Reset();
}


CAlarm::~CAlarm()
{
}

void CAlarm::Reset(void)
{
	alarmTemp = 95.0f;
	histersis = 5.0f;
	isAlarm = false;
	for (uint8_t i = 0; i < MAX_ALARM_CHANNELS; i++)
	{
		achInfo[i].channel = 0xFF;
		achInfo[i].opened = 0;
	}
}

void CAlarm::SetCurrentTemperature(float temp)
{
	if (temp >= alarmTemp)
	{
		isAlarm = true;
		//Lock Relay channels
		for (uint8_t i = 0; i < MAX_ALARM_CHANNELS; i++)
		{
			if (achInfo[i].channel != 0xFF)
			{
				RelayModules.LockChannel(achInfo[i].channel, achInfo[i].opened);
			}
		}
	}
	else if(temp <= alarmTemp-histersis && isAlarm)
	{
		isAlarm = false;
		//Unlock Relay channels
		for (uint8_t i = 0; i < MAX_ALARM_CHANNELS; i++)
		{
			if (achInfo[i].channel != 0xFF)
			{
				RelayModules.UnlockChannel(achInfo[i].channel);
			}
		}
	}
}

bool CAlarm::IsAlarm(void)
{
	return isAlarm;
}

void CAlarm::ApplyConfig(uint8_t cfgPos, const AlarmData & data)
{
	configPos = cfgPos;
	alarmTemp = data.alarm_temp / 100.0f;
	histersis = data.histeresis / 100.0f;
	for (uint8_t i = 0; i < MAX_ALARM_CHANNELS; i++)
	{
		achInfo[i].channel = data.channelInfo[i].channel;
		achInfo[i].opened = data.channelInfo[i].opened;
	}
}

uint8_t CAlarm::GetConfigPos()
{
	return configPos;
}





void CAlarms::Reset(void)
{
	for (uint8_t i = 0; i < Count(); i++)
	{
		CAlarm* al = Get(i);
		if (al)
		{
			al->Reset();
		}
	}
	Clear();
}

CAlarm * CAlarms::GetByConfigPos(uint8_t cfgPos)
{
	for (uint8_t i = 0; i < Count(); i++)
	{
		CAlarm* al = Get(i);
		if (al)
		{
			if (al->GetConfigPos() == cfgPos)
				return al;
		}
	}
	return nullptr;
}

void CAlarms::LoadConfig(void)
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_ALARM_COUNT; i++)
		{
			AlarmData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					CAlarm* al = Add();
					if (al)
					{
						al->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[128];
				sprintf(s, "Alarm config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_ALARM_COUNT; i++)
		{
			const AlarmData& data = Defaults.GetDefaultAlarmData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					CAlarm* al = Add();
					if (al)
					{
						al->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				char s[128];
				sprintf(s, "WaterBoiler config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_ALARM_COUNT; i++)
		{
			const AlarmData& data = Defaults.GetDefaultAlarmData(i);
			if (data.Valid)
			{
				CAlarm* al = Add();
				if (al)
				{
					al->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool CAlarms::GetConfigData(uint8_t pos, AlarmData & data)
{
	if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
	{
		uint32_t fpos = sizeof(VersionInfo) + pos * sizeof(data);
		if (FileManager.FileSeek(fpos))
		{
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				FileManager.FileClose();
				return true;
			}
		}
	}
	return false;
}

bool CAlarms::ValidateSetupDataSet(JsonObject & jo)
{
	return jo.containsKey(jKeyConfigPos) && jo.containsKey(jAlarmTemp) && jo.containsKey(jAlarmHisteresis) && jo.containsKey(jAlarmChannelData);
}

void CAlarms::ParseJson(JsonObject & jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char* action = jo[jKeyAction];
		if (strcmp(action, jKeySetup) == 0)
		{
			if (ValidateSetupDataSet(jo))
			{
				AlarmData data;
				data.alarm_temp = (int)jo[jAlarmTemp];
				data.histeresis = (int)jo[jAlarmHisteresis];
				JsonArray ja = jo[jAlarmChannelData];
				for (uint8_t i = 0; i < MAX_ALARM_CHANNELS; i++)
				{
					JsonObject joo = ja[i];
					data.channelInfo[i].channel = joo[jAlarmChannel];
					data.channelInfo[i].opened = joo[jAlarmOpen];
				}

				if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
				{
					uint8_t cfgPos = jo[jKeyConfigPos];
					uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
					if (FileManager.FileSeek(fpos))
					{
						if (FileManager.FileWriteBuff(&data, sizeof(data)))
						{
							FileManager.FileClose();
							SystemStatus.SetRebootRequired();
							PrintJson.PrintResultOk(jTargetAlarm, jKeySetup, true);
							return;
						}
					}
				}
				PrintJson.PrintResultError(jTargetAlarm, jErrorFileOpenError);
			}
		}
	}
}

CAlarms Alarms;