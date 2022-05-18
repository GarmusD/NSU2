#include "WaterBoilers.h"

const int CWaterBoilers::CURRENT_CONFIG_VERSION = 1;
const VersionInfo CWaterBoilers::VINFO = { 'W', 'T', 'B', CURRENT_CONFIG_VERSION };
const char* CWaterBoilers::CFG_FILE = "wtrboilr.cfg";

CWaterBoiler::CWaterBoiler(void)
{
	Reset();
}

void CWaterBoiler::Begin()
{
	swth = Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_HOT_WATER_MODE]);
	swth->AddStatusChangeHandler(this);
	HandleStatusChange(swth, swth->GetStatus());

	Timers.AddMinuteStartHandler(this, 1);
}

CWaterBoiler::~CWaterBoiler(void)
{
}

void CWaterBoiler::Reset()
{
	enabled = true;
	epenabled = false;
	epoweron = false;
	extcontrol = false;
	epowerchannel = 0xFF;
	memset(epdataa, 0, sizeof(ElHeatingData) * MAX_WATERBOILER_EL_HEATING_DATA_COUNT);

	strlcpy(name, "default", MAX_NAME_LENGTH);
	trigger = NULL;
	cpump = NULL;
	swth = NULL;
	Events::Reset();
}

void CWaterBoiler::ApplyConfig(uint8_t cfgPos, const WaterBoilerData & data)
{
	SetName(data.Name);
	SetCircPump(CirculationPumps.GetByName(data.CircPumpName));
	SetTempTrigger(TempTriggers.GetByName(data.TempTriggerName));
	epenabled = data.ElHeatingEnabled;
	epowerchannel = data.ElHeatingChannel;
	for (uint8_t i = 0; i < MAX_WATERBOILER_EL_HEATING_DATA_COUNT; i++)
	{
		SetElPowerData(i, data.HeatingData[i]);
	}
}

void CWaterBoiler::SetName(const char * n)
{
	strlcpy(name, n, MAX_NAME_LENGTH);
}

char* CWaterBoiler::GetName()
{
	return name;
}

void CWaterBoiler::SetCircPump(CirculationPump* value)
{
	if(value != NULL)
	{
		cpump = value;
	}
}

void CWaterBoiler::StartCircPump()
{
	if(cpump != NULL && !cpump->IsRunning())
	{
		cpump->StartPump();
	}
}

void CWaterBoiler::StopCircPump()
{
	if(cpump != NULL && cpump->IsRunning())
	{
		cpump->StopPump();
	}
}

void CWaterBoiler::SetElPowerData(uint8_t pos, ElHeatingData data)
{
	if (pos < 7)
	{
		epdataa[pos].StartHour = data.StartHour;
		epdataa[pos].StartMin = data.StartMin;
		epdataa[pos].EndHour = data.EndHour;
		epdataa[pos].EndMin = data.EndMin;
	}
}

void CWaterBoiler::SetTempTrigger(TempTrigger* value)
{
	if(trigger != NULL)
	{
		trigger->RemoveStatusChangeHandler(this);
		trigger = NULL;
	}

	if(value != NULL)
	{
		trigger = value;
		trigger->AddStatusChangeHandler(this);
		HandleStatusChange(trigger, trigger->GetStatus());
	}
}

void CWaterBoiler::SetElPowerEnabled(bool value)
{
	epenabled = value;
	HandleTimerEvent(1);
}

void CWaterBoiler::SetExternalControl(bool value)
{
	extcontrol = value;
	if(!extcontrol)
	{
		Log.debug("WaterBoiler external control disabled.");
		HandleStatusChange(swth, swth->GetStatus());
		HandleStatusChange(trigger, trigger->GetStatus());
		HandleTimerEvent(1);
	}
	else
	{
		Log.debug("WaterBoiler external control enabled.");
	}
}


bool CWaterBoiler::TimeInRange(uint8_t sh, uint8_t sm, uint8_t eh, uint8_t em, uint8_t ch, uint8_t cm)
{
	
	uint32_t stime = sh * 1000 + sm * 100;
	uint32_t etime = eh * 1000 + em * 100;
	uint32_t ctime = ch * 1000 + cm * 100;
	if( ctime >= stime && ctime <= etime)
	{
		return true;
	}
	return false;
}

void CWaterBoiler::HandleStatusChange(void* Sender, Status status)
{
	if(extcontrol)
	{
		Log.debug("WaterBoiler::HandleStatusChange - Switch: WaterBoiler in EXTCONTROL. Exiting!");
		return;
	}
	if(Sender == swth)//Swith status change
	{
		Log.debug("WaterBoiler. Handling switch StatusChange.");
		switch (status)
		{
		case STATUS_OFF:
			enabled = false;
			if(cpump && cpump->IsRunning()) cpump->StopPump();
			break;
		case STATUS_ON:
			enabled = true;
			if(trigger != NULL)
			{
				HandleStatusChange(trigger, trigger->GetStatus());
			}
			break;
		case STATUS_DISABLED_OFF:
			enabled = false;
			if(cpump && cpump->IsRunning()) cpump->StopPump();
			break;
		case STATUS_DISABLED_ON:
			enabled = true;
			if(trigger != NULL)
			{
				HandleStatusChange(trigger, trigger->GetStatus());
			}
			break;
		default:
			break;
		}
		if(enabled)
		{
			Log.debug("Water boiler is enabled.");
		}
		else
		{
			Log.debug("Water boiler is disabled.");
		}
	}
	else if(Sender == trigger)//Temp trigger status change
	{
		if(cpump == NULL) return;
		bool ctt = status == STATUS_ON;// CheckTempTriggers();
		if(ctt)
		{
			if(enabled && !cpump->IsRunning())
			{
				Log.debug("Starting WaterBoiler CircPump by TempTrigger.");
				cpump->StartPump();
			}
		}
		else
		{
			if(cpump->IsRunning())
			{
				Log.debug("Stopping WaterBoiler CircPump by TempTrigger.");
				cpump->StopPump();
			}
		}
	}
	else
	{
		Log.error("WaterBoiler: bbz kas kviecia HandleStatusChnage!");
	}
}

void CWaterBoiler::HandleTimerEvent(int te_id)
{
	if(extcontrol) return;
	if(te_id == 1)
	{
		if(!epenabled)
		{
			if(epoweron)
			{
				epoweron = false;
				RelayModules.CloseChannel(epowerchannel);
			}
			return;
		}
		uint8_t h = rtc.getHours();
		uint8_t m = rtc.getMinutes();
		uint8_t wday = rtc.weekDay(rtc.getTimestamp());
		epdata = epdataa[wday-1];

		if(!epenabled)
		{
			if(epoweron)
			{
				//close relay channel
				RelayModules.CloseChannel(epowerchannel);
				epoweron = false;
			}
			return;
		}

		if(TimeInRange(epdata.StartHour, epdata.StartMin, epdata.EndHour, epdata.EndMin, h, m))
		{
			if(!epoweron)
			{
				epoweron = true;
				//Open relay channel
				RelayModules.OpenChannel(epowerchannel);
			}
		}
		else
		{
			if(epoweron)
			{
				epoweron = false;
				RelayModules.CloseChannel(epowerchannel);
			}
		}
	}
}


/*********************************************
*********************************************/

CWaterBoiler* CWaterBoilers::GetByName(const char* name)
{
	CWaterBoiler* boiler;
	for (int i = 0; i<Count(); i++)
	{
		boiler = Get(i);
		if (strcmp(name, boiler->GetName()) == 0) return boiler;
	}
	return NULL;
}

void CWaterBoilers::Begin()
{
	for (int i = 0; i < Count(); i++)
	{
		CWaterBoiler* boiler = Get(i);
		if(boiler) boiler->Begin();
	}
}

void CWaterBoilers::Reset()
{
	Clear();
}

void CWaterBoilers::LoadConfig()
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_WATERBOILER_COUNT; i++)
		{
			WaterBoilerData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					CWaterBoiler* wb = Add();
					if (wb)
					{
						wb->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[64];
				sprintf(s, "WaterBoiler config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_WATERBOILER_COUNT; i++)
		{
			const WaterBoilerData& data = Defaults.GetDefaultWaterBoilerData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					CWaterBoiler* wb = Add();
					if (wb)
					{
						wb->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				char s[64];
				sprintf(s, "WaterBoiler config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_WATERBOILER_COUNT; i++)
		{
			const WaterBoilerData& data = Defaults.GetDefaultWaterBoilerData(i);
			if (data.Valid)
			{
				CWaterBoiler* wb = Add();
				if (wb)
				{
					wb->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool CWaterBoilers::GetConfigData(uint8_t cfgPos, WaterBoilerData & data)
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

bool CWaterBoilers::ValidateSetupDataSet(JsonObject & jo)
{
	return jo.containsKey(jKeyConfigPos) &&
		jo.containsKey(jKeyEnabled) && 
		jo.containsKey(jKeyName) && 
		jo.containsKey(jWaterBoilerTempSensorName) && 
		jo.containsKey(jWaterBoilerTTriggerName) && 
		jo.containsKey(jWaterBoilerCPumpName) && 
		jo.containsKey(jWaterBoilerElHeatingEnabled) && 
		jo.containsKey(jWaterBoilerElPowerChannel) && 
		jo.containsKey(jWaterBoilerElHeatingData);
}

void CWaterBoilers::ParseJson(JsonObject & jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char * action = jo[jKeyAction];
		if (strcmp(action, jKeySetup) == 0)
		{
			if (ValidateSetupDataSet(jo))
			{
				WaterBoilerData data;
				data.Valid = jo[jKeyEnabled];
				Utilities::ClearAndCopyString(jo[jKeyName], data.Name);
				Utilities::ClearAndCopyString(jo[jWaterBoilerTempSensorName], data.TSensorName);
				Utilities::ClearAndCopyString(jo[jWaterBoilerTTriggerName], data.TempTriggerName);
				Utilities::ClearAndCopyString(jo[jWaterBoilerCPumpName], data.CircPumpName);
				data.ElHeatingEnabled = jo[jWaterBoilerElHeatingEnabled];
				data.ElHeatingChannel = jo[jWaterBoilerElPowerChannel];
				JsonArray ja = jo[jWaterBoilerElHeatingData];
				for (uint8_t i = 0; i < MAX_WATERBOILER_EL_HEATING_DATA_COUNT; i++)
				{
					JsonObject joo;
					joo = ja[i];
					data.HeatingData[i].StartHour = joo[jWaterBoilerEHStartHour];
					data.HeatingData[i].StartMin = joo[jWaterBoilerEHStartMin];
					data.HeatingData[i].EndHour = joo[jWaterBoilerEHStopHour];
					data.HeatingData[i].EndMin = joo[jWaterBoilerEHStopMin];
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
							PrintJson.PrintResultOk(jTargetWaterBoiler, action, true);
							return;
						}
					}
					PrintJson.PrintResultError(jTargetWaterBoiler, jErrorFileWriteError);
					return;
				}
				PrintJson.PrintResultError(jTargetWaterBoiler, jErrorFileOpenError);
				return;
			}
			else
			{
				PrintJson.PrintResultError(jTargetWaterBoiler, jErrorInvalidDataSet);
			}
		}
		else
		{
			PrintJson.PrintResultUnknownAction(jTargetWaterBoiler, action);
		}
	}
	else
	{
		PrintJson.PrintResultFormatError();
	}
}

CWaterBoilers WaterBoilers;