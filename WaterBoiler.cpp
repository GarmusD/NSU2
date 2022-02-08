#include "WaterBoiler.h"


CWaterBoiler::CWaterBoiler(void)
{
	enabled = true;
	epenabled = true;
	epoweron = false;
	extcontrol = false;
	epowerchannel = 0xFF;
	epdataa[0].Enabled = false;
	epdataa[1].Enabled = false;
	epdataa[2].Enabled = false;
	epdataa[3].Enabled = false;
	epdataa[4].Enabled = false;
	epdataa[5].Enabled = false;
	epdataa[6].Enabled = false;

	strlcpy(name, "default", MAX_NAME_LENGTH);
	trigger = NULL;
	cpump = NULL;
	swth = NULL;
}

void CWaterBoiler::Begin()
{
	swth = Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_HOT_WATER_MODE].c_str());
	swth->AddStatusChangeHandler(this);
	HandleStatusChange(swth, swth->GetStatus());

	Timers.AddMinuteStartHandler(this, 1);
}

CWaterBoiler::~CWaterBoiler(void)
{
}

void CWaterBoiler::SetName(char * n)
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

void CWaterBoiler::SetElPowerChannel(byte value)
{
	epowerchannel = value;
}

void CWaterBoiler::SetElPowerData(byte pos, ElHeatingData data)
{
	if(pos < 7) epdataa[pos] = data;
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

/*
bool CWaterBoiler::CheckTempTriggers()
{
	if(triggers.Count() == 0) return true;
	for(int i=0; i < triggers.Count(); i++)
	{
		if(!triggers.Get(i)->Check()) return false;
	}
	return true;
}
*/
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
		byte wday = rtc.weekDay(rtc.getTimestamp());
		epdata = epdataa[wday-1];

		if(!epdata.Enabled)
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


CWaterBoilers WaterBoilers;