#include "CirculationPumps.h"
#include "ArduinoJson.h"

const int CCirculationPumps::CURRENT_CONFIG_VERSION = 2;
const VersionInfo CCirculationPumps::VINFO = { 'C', 'P', 'S', CURRENT_CONFIG_VERSION };
const char* CCirculationPumps::CFG_FILE = "crcpumps.cfg";

CirculationPump::CirculationPump(void)
{
	trigger = NULL;
	Init();
}

CirculationPump::~CirculationPump(void)
{
}

void CirculationPump::Init()
{
	configPos = 0xFF;
	counter = 0;
	running = false;
	channel = 0xFF;
	manual = false;
	status = STATUS_OFF;
	enabled = true;
	max_speed = 1;
	current_speed = 0;
	total_valves = 0;
	speed_channels[0] = 0xFF;
	speed_channels[1] = 0xFF;
	speed_channels[2] = 0xFF;
	standalone = false;
	trigger = NULL;
	memset(name, 0, MAX_NAME_LENGTH);
	Events::Reset();
}

void CirculationPump::Reset()
{
	Init();
}

uint8_t CirculationPump::GetConfigPos()
{
	return configPos;
}

void CirculationPump::ApplyConfig(uint8_t cfgPos, const CircPumpData & data)
{
	configPos = cfgPos;
	SetName(data.Name);
	SetSpeed(data.MaxSpeed, data.Spd1Channel, data.Spd2Channel, data.Spd3Channel);
	switch_speed();
	if (strlen(data.TempTriggerName))
	{
		SetTempTrigger(TempTriggers.GetByName(data.TempTriggerName));
	}
}

void CirculationPump::ChangeStatus(Status new_status)
{
	status = new_status;
	PrintStatus();
}

void CirculationPump::PrintStatus(const char * action)
{
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
	root[jKeyTarget] = jTargetCircPump;
	if (action[0] == 0)
	{
		root[jKeyAction] = jValueInfo;
	}
	else
	{
		root[jKeyAction] = action;
	}
	root[jKeyName] = name;
	root[jKeyStatus] = Events::GetStatusName(status);
	root[jCircPumpCurrentSpeed] = current_speed;
	root[jCircPumpValvesOpened] = counter;

	PrintJson.Print(root);
}

int CirculationPump::GetOpenedValveCount()
{
	return counter;
}

int CirculationPump::GetCurrentSpeed()
{
	return current_speed;
}

bool CirculationPump::IsRunning()
{
	return running;
}

bool CirculationPump::IsEnabled(){
	return enabled;
}

void CirculationPump::ValveOpened()
{
	counter++;
	char s[128];
	sprintf(s, "CirculationPump (%s)::ValveOpened() - valves count: %d", name, counter);
	Log.debug(s);
	switch_speed();
	if (!manual && !running)
	{
		StartPump();
	}
	else
	{
		PrintStatus();
	}
}

void CirculationPump::ValveClosed()
{
	counter--;
	char s[128];
	sprintf(s, "CirculationPump (%s)::ValveClosed() - valves left: %d", name, counter);
	Log.debug(s);
	if(counter < 0) counter = 0;
	switch_speed();
	if (counter == 0 && !manual && running)
	{
		StopPump();
	}
	else
	{
		PrintStatus();
	}
}

void CirculationPump::StartPump()
{
	if(channel != 0xFF && enabled && !running)
	{
		running = true;
		char s[128];
		sprintf(s, "CirculationPump (%s)::StartPump() - Ch: %d. Opened valves: %d", name, channel, counter);
		Log.debug(s);		
		if(!manual)
		{
			ChangeStatus(STATUS_ON);
			DispatchStatusChange(this, status);
		}
		RelayModules.OpenChannel(channel);
	}
}

void CirculationPump::StopPump()
{
	if(channel !=0xFF && running)
	{		
		if(!manual)
		{
			char s[128];
			sprintf(s, "CirculationPump (%s)::StopPump() - Ch: %d. Opened valves: %d", name, channel, counter);
			Log.debug(s);
			running = false;			
			if(status != STATUS_DISABLED){
				ChangeStatus(STATUS_OFF);
				DispatchStatusChange(this, status);
			}
			RelayModules.CloseChannel(channel);
		}
		else
		{
			char s[128];
			sprintf(s, "CirculationPump ($s)::StopPump() - Ch: %d - MANUAL MODE, NOT STOPPING. Opened valves: %d", name, channel, counter);
			Log.debug(s);
		}
	}
}

void CirculationPump::DisablePump()
{
	if (channel != 0xFF && running)
	{
		if (!manual)
		{
			char s[128];
			sprintf(s, "CirculationPump (%s)::DisablePump() - Ch: %d. Opened valves: %d", name, channel, counter);
			Log.debug(s);
			running = false;
			if (status != STATUS_DISABLED) {
				ChangeStatus(STATUS_DISABLED);
				DispatchStatusChange(this, status);
			}
			RelayModules.CloseChannel(channel);
		}
		else 
		{
			char s[128];
			sprintf(s, "CirculationPump (%s)::DisablePump() - Ch: %d - MANUAL MODE, NOT STOPPING. Opened valves: %d", name, channel, counter);
			Log.debug(s);
		}
	}
}

void CirculationPump::SwitchManualMode()
{
	manual = !manual;
	char s[128];
	sprintf(s, "CirculationPump %s ManualMode %s", name, manual ? "true" : "false");
	Log.debug(s);
	if(manual)
	{
		if(channel != 0xFF)
		{
			running = true;
			Timers.AddTimerHandler(this, 0xDD, MINUTES15, true);
			ChangeStatus(STATUS_MANUAL);
			RelayModules.OpenChannel(channel);
			DispatchStatusChange(this, status);
		}
	}
	else
	{
		if(channel != 0xFF)
		{
			if(counter == 0) 
			{
				running = true;
				StopPump();
			}
			else if(enabled)
			{
				running = false;
				StartPump();
			}else
			{
				running = true;
				DisablePump();				
			}
		}
	}
}

void CirculationPump::SetName(const char* name)
{
	strlcpy(this->name, name, MAX_NAME_LENGTH);
}

const char* CirculationPump::GetName()
{
	return name;
}

void CirculationPump::SetSpeed(uint8_t maxspeed, uint8_t speed1_channel, uint8_t speed2_channel, uint8_t speed3_channel)
{
	max_speed = maxspeed;
	if(max_speed < 1) max_speed = 1;
	if(max_speed > 3) max_speed = 3;
	speed_channels[0] = speed1_channel;
	speed_channels[1] = speed2_channel;
	speed_channels[2] = speed3_channel;
	calc_speed_table();
}

void CirculationPump::RegisterValveCount(uint8_t valvecount)
{
	char s[128];
	sprintf(s, "CirculationPump::RegisterValveCount(%d)", valvecount);
	Log.debug(s);
	if (valvecount == 0xFF)
	{
		Log.debug("Value is not correct. No valves registered.");
		return;
	}
	total_valves += valvecount;
	calc_speed_table();
}

void CirculationPump::calc_speed_table()
{
	if(total_valves <= 0) return;
	if(max_speed == 1){
		speed_table[0] = total_valves;
	}else if(max_speed == 2){
		uint8_t valves_per_speed = total_valves / 2;
		speed_table[0] = valves_per_speed;
		speed_table[1] = total_valves;
		speed_table[2] = 0xFF;
	}else if (max_speed == 3){
		 uint8_t valves_per_speed = total_valves / 3;
		speed_table[0] = valves_per_speed;
		speed_table[1] = valves_per_speed * 2;
		speed_table[2] = total_valves;
	}
}

void CirculationPump::switch_speed()
{
	uint8_t spd = getReqSpeed();
	if(spd != current_speed){
		current_speed = spd;
		if(speed_channels[0] != 0xFF) RelayModules.CloseChannel(speed_channels[0]);
		if(speed_channels[1] != 0xFF) RelayModules.CloseChannel(speed_channels[1]);
		if(speed_channels[2] != 0xFF) RelayModules.CloseChannel(speed_channels[2]);
		if(speed_channels[speed_channels[spd-1]] != 0xFF) {
			channel = speed_channels[spd - 1];
			if(channel != 0xFF && running)
				RelayModules.OpenChannel(channel);
		}
	}
}

uint8_t CirculationPump::getReqSpeed()
{
	uint8_t spd = max_speed;
	if(counter <= speed_channels[0]){
		spd = 1;
	}else if(counter <= speed_channels[1]){
		spd = 2;
	}else if(counter <= speed_channels[2]){
		spd = 3;
	}
	if(spd > max_speed)
		spd = max_speed;
	return spd;
}

void CirculationPump::SetTempTrigger(TempTrigger* value)
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

void CirculationPump::HandleStatusChange(void* Sender, Status value)
{
	if(Sender == trigger)
	{
		//bool new_state = 
		enabled = value == STATUS_ON;// CheckTempTriggers();
		char s[128];
		sprintf(s, "CirculationPump %s in HandleStatusChange Trigger value: %s", name, Events::GetStatusName(value));
		Log.debug(s);
		sprintf(s, "CirculationPump %s StatusChange ", name, enabled ? "true":"false");
		Log.debug(s);
		if(status == STATUS_MANUAL) return;
		if(!enabled)
		{
			sprintf(s, "CirculationPump %s is disabled.", name);
			Log.debug(s);
			if(status != STATUS_DISABLED){
				ChangeStatus(STATUS_DISABLED);
				DisablePump();				
				DispatchStatusChange(this, status);
			}
		}
		else //enabled
		{
			if(status == STATUS_DISABLED)
			{
				if(counter) 
				{
					sprintf(s, "CirculationPump %s. Starting circpump", name);
					Log.debug(s);
					StartPump();
				}
				else
				{
					ChangeStatus(STATUS_OFF);
					DispatchStatusChange(this, status);
				}
			}
			else if(status == STATUS_OFF)
			{
				if(counter) 
				{
					sprintf(s, "CirculationPump %s. Starting circpump", name);
					Log.debug(s);
					StartPump();
				}
				else
				{
					ChangeStatus(STATUS_OFF);
					DispatchStatusChange(this, status);
				}
			}
		}
	}
}

void CirculationPump::HandleTimerEvent(int te_id)
{
	if(te_id == 0xDD)
	{
		if(manual)
		{
			SwitchManualMode();
		}
	}
}

Status CirculationPump::getStatus(){
	return status;
}





CirculationPump* CCirculationPumps::GetByName(const char* name)
{
	CirculationPump* pump;
	for(int i=0; i<Count(); i++)
	{
		pump = Get(i);
		if(strcmp(name, pump->GetName()) == 0) return pump;
	}
	return NULL;
}

CirculationPump * CCirculationPumps::GetByConfigPos(uint8_t cfgPos)
{
	CirculationPump* pump;
	for (int i = 0; i < Count(); i++)
	{
		pump = Get(i);
		if (pump->GetConfigPos() == cfgPos) return pump;
	}
	return nullptr;
}

void CCirculationPumps::Reset()
{
	Clear();
}

void CCirculationPumps::LoadConfig(void)
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_CIRCULATION_PUMPS; i++)
		{
			CircPumpData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					CirculationPump* cp = Add();
					if (cp)
					{
						cp->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[128];
				sprintf(s, "CircPump config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_CIRCULATION_PUMPS; i++)
		{
			const CircPumpData& data = Defaults.GetDefaultCircPumpData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					CirculationPump* cp = Add();
					if (cp)
					{
						cp->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				char s[128];
				sprintf(s, "CircPump config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_CIRCULATION_PUMPS; i++)
		{
			const CircPumpData& data = Defaults.GetDefaultCircPumpData(i);
			if (data.Valid)
			{
				CirculationPump* cp = Add();
				if (cp)
				{
					cp->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool CCirculationPumps::GetConfigData(uint8_t cfgPos, CircPumpData & data)
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

bool CCirculationPumps::ValidateSetupDataSet(JsonObject & jo)
{
	if (jo.containsKey(jKeyConfigPos) &&
		jo.containsKey(jKeyEnabled) &&
		jo.containsKey(jKeyName) &&
		jo.containsKey(jCircPumpTTriggerName) &&
		jo.containsKey(jCircPumpMaxSpeed) &&
		jo.containsKey(jCircPumpSpeed1Ch) &&
		jo.containsKey(jCircPumpSpeed2Ch) &&
		jo.containsKey(jCircPumpSpeed3Ch)
		)
	{
		return true;
	}
	else
	{
		PrintJson.PrintResultError(jTargetCircPump, jErrorInvalidDataSet);
		return false;
	}
}

void CCirculationPumps::ParseJson(JsonObject& jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char * action = jo[jKeyAction];
		if (strcmp(action, jCircPumpActionClick) == 0)
		{
			if (jo.containsKey(jKeyName))
			{
				CirculationPump* cp = GetByName(jo[jKeyName]);
				if (cp != NULL)
				{
					cp->SwitchManualMode();
				}
			}
			else
			{
				PrintJson.PrintResultFormatError();
			}
		}
		else if (strcmp(action, jKeySetup) == 0)
		{
			if (ValidateSetupDataSet(jo))
			{
				CircPumpData data;
				data.Valid = jo[jKeyEnabled];
				Utilities::ClearAndCopyString(jo[jKeyName], data.Name);
				Utilities::ClearAndCopyString(jo[jCircPumpTTriggerName], data.TempTriggerName);
				data.MaxSpeed = jo[jCircPumpMaxSpeed];
				data.Spd1Channel = jo[jCircPumpSpeed1Ch];
				data.Spd2Channel = jo[jCircPumpSpeed2Ch];
				data.Spd2Channel = jo[jCircPumpSpeed3Ch];

				uint8_t cfgPos = jo[jKeyConfigPos];
				uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
				if (FileManager.OpenConfigFile(CFG_FILE, VINFO))
				{
					if (FileManager.FileSeek(fpos))
					{
						if (FileManager.FileWriteBuff(&data, sizeof(data)))
						{
							SystemStatus.SetRebootRequired();
							PrintJson.PrintResultOk(jTargetCircPump, action, true);
							return;
						}
					}
					PrintJson.PrintResultError(jTargetCircPump, jErrorFileWriteError);
					return;
				}
				PrintJson.PrintResultError(jTargetCircPump, jErrorFileOpenError);
				return;
			}

		}
	}
	else
	{
		PrintJson.PrintResultFormatError();
	}
}

CCirculationPumps CirculationPumps;