#include "CirculationPump.h"
#include "ArduinoJson.h"

CirculationPump::CirculationPump(void)
{
	this->counter = 0;
	this->running = false;
	channel = 0xFF;
	//rm = RelayModule::getInstance();
	manual = false;
	status = STATUS_OFF;
	enabled = true;
	max_speed = 1;
	current_speed = 1;
	total_valves = 0;
	speed_channels[0] = 0xFF;
	speed_channels[1] = 0xFF;
	speed_channels[2] = 0xFF;
	standalone = false;
	trigger = NULL;
}


CirculationPump::~CirculationPump(void)
{
}

void CirculationPump::ChangeStatus(Status new_status)
{
	status = new_status;
	PrintStatus();
}

void CirculationPump::PrintStatus(String action)
{
	StaticJsonBuffer<512> jBuff;
	JsonObject& root = jBuff.createObject();
	root[jKeyTarget] = jTargetCircPump;
	if (action == "")
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

String CirculationPump::GetInfoString()
{
	return "INFO: CIRCPUMP: "+String(name)+" "+GetStatusName(status);
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
	Log.debug("CirculationPump (" + String(name) + ")::ValveOpened() - valves count: " + String(counter));
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
	Log.debug("CirculationPump (" + String(name) + ")::ValveClosed() - valves left: "+String(counter));
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
		Log.debug("CirculationPump (" + String(name) + ")::StartPump() - Ch: " + String(channel) + ". Opened valves: "+String(counter));		
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
			Log.debug("CirculationPump (" + String(name) + ")::StopPump() - Ch: " + String(channel) + ". Opened valves: "+String(counter));
			running = false;			
			if(status != STATUS_DISABLED){
				ChangeStatus(STATUS_OFF);
				DispatchStatusChange(this, status);
			}
			RelayModules.CloseChannel(channel);
		}else{
			Log.debug("CirculationPump (" + String(name) + ")::StopPump() - Ch: " + String(channel) + " - MANUAL MODE, NOT STOPPING. Opened valves: "+String(counter));
		}
	}
}

void CirculationPump::DisablePump()
{
	if (channel != 0xFF && running)
	{
		if (!manual)
		{
			Log.debug("CirculationPump (" + String(name) + ")::DisablePump() - Ch: " + String(channel) + ". Opened valves: " + String(counter));
			running = false;
			if (status != STATUS_DISABLED) {
				ChangeStatus(STATUS_DISABLED);
				DispatchStatusChange(this, status);
			}
			RelayModules.CloseChannel(channel);
		}
		else {
			Log.debug("CirculationPump (" + String(name) + ")::DisablePump() - Ch: " + String(channel) + " - MANUAL MODE, NOT STOPPING. Opened valves: " + String(counter));
		}
	}
}

void CirculationPump::SetRelayChannel(int ch)
{
	channel = ch;
}

void CirculationPump::SwitchManualMode()
{
	manual = !manual;
	Log.debug("CirculationPump " + String(name) + " ManualMode " + String(manual));
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

void CirculationPump::SetMaxSpeed(byte maxspeed, byte speed1_channel, byte speed2_channel, byte speed3_channel)
{
	max_speed = maxspeed;
	if(max_speed < 1) max_speed = 1;
	if(max_speed > 3) max_speed = 3;
	speed_channels[0] = speed1_channel;
	speed_channels[1] = speed2_channel;
	speed_channels[2] = speed3_channel;
	calc_speed_table();
}

void CirculationPump::RegisterValveCount(byte valvecount)
{
	Log.debug("CirculationPump::RegisterValveCount("+String(valvecount)+")");
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
		byte valves_per_speed = total_valves / 2;
		speed_table[0] = valves_per_speed;
		speed_table[1] = total_valves;
		speed_table[2] = 0xFF;
	}else if (max_speed == 3){
		 byte valves_per_speed = total_valves / 3;
		speed_table[0] = valves_per_speed;
		speed_table[1] = valves_per_speed * 2;
		speed_table[2] = total_valves;
	}
}

void CirculationPump::switch_speed()
{
	byte spd = getReqSpeed();
	if(spd != current_speed){
		current_speed = spd;
		if(speed_channels[0] != 0xFF) RelayModules.CloseChannel(speed_channels[0]);
		if(speed_channels[1] != 0xFF) RelayModules.CloseChannel(speed_channels[1]);
		if(speed_channels[2] != 0xFF) RelayModules.CloseChannel(speed_channels[2]);
		if(speed_channels[speed_channels[spd-1]] != 0xFF) {
			RelayModules.OpenChannel(speed_channels[spd-1]);
		}
	}
}

byte CirculationPump::getReqSpeed()
{
	byte spd = max_speed;
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
/*
bool CirculationPump::CheckTempTriggers()
{
	if(triggers.Count() == 0) return true;
	for(int i=0; i < triggers.Count(); i++)
	{
		if(!triggers.Get(i)->Check()) return false;
	}
	return true;
}
*/
void CirculationPump::HandleStatusChange(void* Sender, Status value)
{
	if(Sender == trigger)
	{
		//bool new_state = 
		enabled = value == STATUS_ON;// CheckTempTriggers();
		Log.debug("CirculationPump " + String(name) + " in HandleStatusChange Trigger value: " +Events::GetStatusName(value));
		Log.debug("CirculationPump " + String(name) + " StatusChange " + String(enabled));
		if(status == STATUS_MANUAL) return;
		if(!enabled)
		{
			Log.debug("CirculationPump " + String(name) + " is disabled.");
			if(status != STATUS_DISABLED){
				ChangeStatus(STATUS_DISABLED);
				DisablePump();				
				DispatchStatusChange(this, status);
			}
		}
		else //enabled
		{
			if(status == STATUS_DISABLED){
				if(counter) {
					Log.debug("CirculationPump " + String(name) + " Starting circpump");
					StartPump();
				}else{
					ChangeStatus(STATUS_OFF);
					DispatchStatusChange(this, status);
				}
			}
			else if(status == STATUS_OFF)
			{
				if(counter) {
					Log.debug("CirculationPump " + String(name) + " Starting circpump");
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

void CirculationPump::SetStandalone(bool value){
	standalone = value;
	if(standalone)
	{
		counter++;
	}
	else
	{
		counter--;
	}
	HandleStatusChange(this, STATUS_UNKNOWN);
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

void CCirculationPumps::ParseJson(JsonObject& jo)
{
	if (jo.containsKey(jKeyName))
	{
		CirculationPump* cp = GetByName(jo[jKeyName]);
		if (cp != NULL)
		{
			if (jo.containsKey(jKeyAction))
			{
				String action = jo[jKeyAction];
				if (action.equals(jCircPumpActionClick))
				{
					cp->SwitchManualMode();
				}
			}
		}
	}
}

CCirculationPumps CirculationPumps;