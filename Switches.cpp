#include "switches.h"
#include "Settings.h"

Switch::Switch(void)
{
	Init();
}


Switch::~Switch(void)
{
}


const char* Switch::GetName()
{
	return name;
}

void Switch::SwitchState()
{
	if(isForced)
	{
		Log.debug("Switch '"+String(name)+"': Cannot switch state when forced.");
		return;
	}
	if(status == STATUS_ON)
	{
		status = STATUS_OFF;
		SaveStatus();
		PrintStatus();
		DispatchStatusChange(this, status);
	}else if(status == STATUS_OFF)
	{
		status = STATUS_ON;
		SaveStatus();
		PrintStatus();
		DispatchStatusChange(this, status);
	}
	else
	{
		Log.error("Switch '"+String(name)+"' has incorrect state: "+GetStatusName(status));
	}
}

void Switch::SetStatus(Status value)
{
	Log.debug("Trying to set for switch '"+String(name)+"' Status: "+GetStatusName(value));
	switch(value){
	case STATUS_DISABLED_OFF:
	case STATUS_DISABLED_ON:
	case STATUS_OFF:
	case STATUS_ON:
		if(status != value)
		{
			if(!isForced)
			{
				status = value;
				SaveStatus();
			}
			PrintStatus();
			DispatchStatusChange(this, status);
		}
		break;
	default:
		Log.error("Setting incorrect state for switch '"+String(name)+"' "+GetStatusName(value));
		break;
	}
}

Status Switch::GetStatus()
{
	if(!isForced)
	{
		return status;
	}
	else
	{
		return forced_status;
	}
}

void Switch::SaveStatus()
{
	EEPROM.SetByte(EEPROM_POS_SWITCH[config_pos], (byte)status);
}

void Switch::AddDependance(Switch* s, Status OnStatus, Status myStatus)
{
	depend = s;
	if (depend)
	{
		onStatus = OnStatus;
		forced_status = myStatus;
		if (forced_status == STATUS_ON) forced_status = STATUS_DISABLED_ON;
		if (forced_status == STATUS_OFF) forced_status = STATUS_DISABLED_OFF;

		depend->AddStatusChangeHandler(this);
		HandleStatusChange(depend, depend->GetStatus());
	}
}

void Switch::HandleStatusChange(void* Sender, Status value)
{
	if(depend && Sender == depend)
	{
		if(value == onStatus && !isForced)
		{
			Log.debug("Switch '"+String(name)+"' is forced state to "+GetStatusName(forced_status));
			isForced = true;
			PrintStatus();
			DispatchStatusChange(this, forced_status);
		}
		else if(isForced)
		{
			Log.debug("Force for Switch '"+String(name)+"' is disabled. Returning to old state "+GetStatusName(status));
			isForced = false;
			PrintStatus();
			DispatchStatusChange(this, status);
		}
	}
}

bool Switch::IsForced()
{
	return isForced;
}

String Switch::GetInfoString()
{
	if(isForced)
	{
		return "INFO: SWITCH: "+String(name)+" "+GetStatusName(forced_status) + " " + String((int)isForced);
	}
	else
	{
		return "INFO: SWITCH: "+String(name)+" "+GetStatusName(status) + " " + String((int)isForced);
	}
}

void Switch::Init()
{
	status = STATUS_ON;
	forced_status = STATUS_UNKNOWN;
	onStatus = STATUS_UNKNOWN;
	depend = NULL;
	config_pos = 255;
	name[0] = 0;
	isForced = false;
}

void Switch::ApplySettings(byte cfgPos, const SwitchData& data)
{
	config_pos = cfgPos;
	strlcpy(name, data.Name, MAX_NAME_LENGTH);
	SetStatus((Status)EEPROM.GetByte(EEPROM_POS_SWITCH[config_pos]));
	if (data.DependOn[0] != 0)
	{
		AddDependance(Switches.GetByName(data.DependOn), (Status)data.depstate, (Status)data.forcestate);
	}
}

void Switch::WriteSettings()
{
}

void Switch::CleanUp()
{
	Init();
}

void Switch::PrintStatus()
{
	StaticJsonBuffer<512> jBuff;
	JsonObject& root = jBuff.createObject();
	root[jKeyTarget] = jTargetSwitch;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	root[jKeyStatus] = Events::GetStatusName(GetStatus());
	root[jSwitchIsForced] = isForced;
	PrintJson.Print(root);
}

Switch* CSwitches::GetByName(const char* name)
{
	Switch* swth;
	for(int i=0; i<Count(); i++)
	{
		swth = Get(i);
		if(strcmp(name, swth->GetName()) == 0) return swth;
	}
	return NULL;
}

void CSwitches::ParseJson(JsonObject& jo)
{
	if (jo.containsKey(jKeyName))
	{
		Switch* sw = GetByName(jo[jKeyName]);
		if (sw)
		{
			if (jo.containsKey(jKeyAction))
			{
				String action = jo[jKeyAction];
				if (action.equals(jSwitchClick))
				{
					sw->SwitchState();
					return;
				}
			}
		}
	}
}
CSwitches Switches;
