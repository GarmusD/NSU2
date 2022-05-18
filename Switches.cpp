#include "switches.h"
#include "SystemStatus.h"
#include "Utilities.h"


const int CSwitches::CURRENT_CONFIG_VERSION = 1;
const VersionInfo CSwitches::VINFO = { 'S', 'W', 'T', CURRENT_CONFIG_VERSION };
const char* CSwitches::CFG_FILE = "switches.cfg";


Switch::Switch(void)
{
	Init();
}


Switch::~Switch(void)
{
}

void Switch::Begin()
{
	loading = false;
	if (delayedSaveState)
	{
		SaveStatus();
		delayedSaveState = false;
	}
}

const char* Switch::GetName()
{
	return name;
}

uint8_t Switch::GetConfigPos()
{
	return configPos;
}

void Switch::SwitchState()
{
	if(isForced)
	{
		char s[64];
		sprintf(s, "Switch '%s': Cannot switch state when forced.", name);
		Log.debug(s);
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
		char s[64];
		sprintf(s, "Switch '%s' has incorrect state: %s", name, GetStatusName(status));
		Log.error(s);
	}
}

void Switch::SetStatus(Status value)
{
	char s[64];
	sprintf(s, "Trying to set for switch '%s' Status: %s", name, GetStatusName(value));
	Log.debug(s);
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
		sprintf(s, "Setting incorrect state for switch '%s' %s", name, GetStatusName(value));
		Log.error(s);
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
	if (loading)
	{
		delayedSaveState = true;
		return;
	}
	if (FileManager.OpenConfigFile(CSwitches::CFG_FILE, CSwitches::VINFO) == FileStatus_OK)
	{
		SwitchData data;
		uint32_t fpos = sizeof(VersionInfo) + configPos * sizeof(data);
		if (FileManager.FileSeek(fpos))
		{
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				data.status = status;
				if (FileManager.FileSeek(fpos))
				{
					FileManager.FileWriteBuff(&data, sizeof(data));
					FileManager.FileClose();
					return;
				}
			}			
		}
		char s[64];
		sprintf(s, "Switches config file '%s' read error. ConfigPos: %d", CSwitches::CFG_FILE, configPos);
		Log.error(s);
		FileManager.FileClose();
		return;
	}
	char s[64];
	sprintf(s, "Switches config file '%s' open error.", CSwitches::CFG_FILE);
	Log.error(s);
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
			char s[64];
			sprintf(s, "Switch '%s' is forced state to %s", name, GetStatusName(forced_status));
			Log.debug(s);
			isForced = true;
			PrintStatus();
			DispatchStatusChange(this, forced_status);
		}
		else if(isForced)
		{
			char s[128];
			sprintf(s, "Force for Switch '%s' is disabled. Returning to old state %s", name, GetStatusName(status));
			Log.debug(s);
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

void Switch::Init()
{
	status = STATUS_ON;
	forced_status = STATUS_UNKNOWN;
	onStatus = STATUS_UNKNOWN;
	depend = NULL;
	configPos = 255;
	name[0] = 0;
	isForced = false;
	loading = false;
	delayedSaveState = false;
}

void Switch::ApplyConfig(uint8_t cfgPos, const SwitchData& data)
{
	loading = true;
	configPos = cfgPos;
	strlcpy(name, data.Name, MAX_NAME_LENGTH);
	SetStatus((Status)data.status);
	if (data.DependOn[0] != 0)
	{
		AddDependance(Switches.GetByName(data.DependOn), (Status)data.depstate, (Status)data.forcestate);
	}
}

void Switch::WriteSettings()
{
}

void Switch::Reset()
{
	Init();
}

void Switch::PrintStatus()
{
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
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

Switch* CSwitches::GetByConfigPos(uint8_t cfgPos)
{
	Switch* swth;
	for (int i = 0; i < Count(); i++)
	{
		swth = Get(i);
		if (swth->GetConfigPos() == cfgPos) return swth;
	}
	return NULL;
}

void CSwitches::Begin()
{
	Switch* swth;
	for (uint8_t i = 0; i < Count(); i++)
	{
		swth = Get(i);
		if (swth)
			swth->Begin();
	}
}

void CSwitches::Reset()
{
	Clear();
}

void CSwitches::LoadConfig()
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_SWITCHES_COUNT; i++)
		{
			SwitchData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					Switch *sw = Add();
					if (sw)
					{
						sw->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[64];
				sprintf(s, "Switches config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_SWITCHES_COUNT; i++)
		{
			const SwitchData& data = Defaults.GetDefaultSwitchData(i);
			if (!FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				char s[64];
				sprintf(s, "Switches config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
			if (data.Valid)
			{
				Switch* sw = Add();
				if (sw)
				{
					sw->ApplyConfig(i, data);
				}
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_SWITCHES_COUNT; i++)
		{
			const SwitchData& data = Defaults.GetDefaultSwitchData(i);
			if (data.Valid)
			{
				Switch *sw = Add();
				if (sw)
				{
					sw->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool CSwitches::GetConfigData(uint8_t cfgPos, SwitchData & data)
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

bool CSwitches::ValidateSetupDataSet(JsonObject & jo)
{
	if (jo.containsKey(jKeyConfigPos) && 
		jo.containsKey(jKeyEnabled) &&
		jo.containsKey(jKeyName) &&
		jo.containsKey(jSwitchDependName) &&
		jo.containsKey(jSwitchDependState) &&
		jo.containsKey(jSwitchForceState) &&
		jo.containsKey(jSwitchCurrState))
	{
		return true;
	}
	PrintJson.PrintResultError(jTargetSwitch, jErrorInvalidDataSet);
	return false;
}

void CSwitches::ParseJson(JsonObject & jo)
{
	const char * action = jo[jKeyAction];
	if (strcmp(action, jSwitchClick) == 0)
	{
		if (jo.containsKey(jKeyName))
		{
			Switch* sw = GetByName(jo[jKeyName]);
			if (sw)
			{
				sw->SwitchState();
				return;
			}
		}
		return;
	}
	else if (strcmp(action, jKeySetup) == 0)
	{
		if (ValidateSetupDataSet(jo))
		{
			SwitchData data;
			uint8_t cfgPos = jo[jKeyConfigPos];
			data.Valid = jo[jKeyEnabled];
			Utilities::ClearAndCopyString(jo[jKeyName], data.Name);
			Utilities::ClearAndCopyString(jo[jSwitchDependName], data.DependOn);
			data.depstate = jo[jSwitchDependState];
			data.forcestate = jo[jSwitchForceState];
			data.status = jo[jSwitchCurrState];

			uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
			if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
			{
				if (FileManager.FileSeek(fpos))
				{
					if (FileManager.FileWriteBuff(&data, sizeof(data)))
					{
						FileManager.FileClose();
						SystemStatus.SetRebootRequired();
						PrintJson.PrintResultOk(jTargetSwitch, jKeySetup, true);
						return;
					}
				}
				PrintJson.PrintResultError(jTargetSwitch, jErrorFileOpenError);
				return;
			}
			PrintJson.PrintResultError(jTargetSwitch, jErrorFileWriteError);
		}
	}
	else
	{
		PrintJson.PrintResultUnknownAction(jTargetSwitch, action);
	}
}
CSwitches Switches;
