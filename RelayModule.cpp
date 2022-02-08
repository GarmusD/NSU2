#include <Arduino.h>
#include "RelayModule.h"
#include "nsu_pins.h"
#include <SPI.h>

//RelayModule* RelayModule::_instance = NULL;

CRelayModules::CRelayModules(void)
{	
	channelCount = 0;
	update_pending = false;
	last_rewrite = 0;
}

CRelayModules::~CRelayModules(void)
{
}

void CRelayModules::Init()
{
	pinMode(RM_OE_PIN, INPUT);
	pinMode(RM_OE_PIN, OUTPUT);
	digitalWrite(RM_OE_PIN, HIGH);

	pinMode(RM_LATCH_PIN, INPUT);
	pinMode(RM_LATCH_PIN, OUTPUT);
	digitalWrite(RM_LATCH_PIN, HIGH);
}

void CRelayModules::Begin()
{
	TimeSlice.RegisterTimeSlice(this);
}

void CRelayModules::AddRelayModule(byte activelow, byte inverted)
{
	RelayModuleInfo* info = modules.Add();
	if (info)
	{
		info->activelow = activelow;
		if (info->activelow)
		{
			info->flags = 0xFF;
		}
		else
		{
			info->flags = 0;
		}
		info->inverted = inverted;
	}
}


void CRelayModules::AllChannelsOff()
{
	Log.debug("RelayModuleClass::AllChannelsOff()");
	digitalWrite(RM_OE_PIN, HIGH);
	//delay(10);
	digitalWrite(RM_LATCH_PIN, LOW);
	for(byte i = 0; i < modules.Count(); i++)
	{
		RelayModuleInfo* info = modules.Get(i);
		if (info)
		{
			if (info->activelow)
			{
				info->flags = 0xFF;
			}
			else
			{
				info->flags = 0;
			}
			SPI.transfer(info->flags);
		}
		else
		{
			Log.error("RelayModuleClass::AllChannelsOff() - RelayModuleInfo is NULL.");
		}
	}
	digitalWrite(RM_LATCH_PIN, HIGH);
	//delay(10);
	digitalWrite(RM_OE_PIN, LOW);
	Log.debug("RelayModuleClass::AllChannelsOff() done.");
}

void CRelayModules::WriteChannels()
{
	digitalWrite(RM_OE_PIN, HIGH);
	//delay(10);
	digitalWrite(RM_LATCH_PIN, LOW);
	//Log.debug("RelayModuleClass::WriteChannels(). Modules count: "+String(modules.Count()));
	int cnt = modules.Count();
	for(int i = cnt-1; i >= 0; --i)
	{
		RelayModuleInfo* rmi = modules.Get(i);
		if(rmi != NULL)
		{
			SPI.transfer(rmi->flags);
		}
		else
		{
			//Log.debug("WriteChannels: RelayModuleInfo is NULL. i="+String(i));
		}
		
	}
	digitalWrite(RM_LATCH_PIN, HIGH);
	//delay(10);
	digitalWrite(RM_OE_PIN, LOW);
	//Log.debug("RelayModule: channels writed.");
}

bool CRelayModules::OpenChannel(byte channel)
{
	if (channel == 0xFF)
		return false;
	//Log.info("RELAYMODULE: OpenChannel "+String(channel));
	PrintChannelAction(channel, true);
	channel--;
	if(channel >= 0 && channel < modules.Count()*8)
	{
		byte idx = floor(channel / 8);		
		if(idx < modules.Count())
		{
			RelayModuleInfo* info = modules.Get(idx);
			if (info)
			{
				byte bit = channel - (idx * 8);
				if (info->inverted)
					bit = 7 - bit;
				byte to_set = info->flags;
				if (info->activelow)
				{
					bitClear(to_set, bit);
				}
				else
				{
					bitSet(to_set, bit);
				}
				modules.Get(idx)->flags = to_set;
				update_pending = true;
			}
		}
		//WriteChannels();
	}
	return true;
}

bool CRelayModules::CloseChannel(byte channel)
{
	if (channel == 0xFF)
		return false;
	//Log.info("RELAYMODULE: CloseChannel "+String(channel));
	PrintChannelAction(channel, false);
	channel--;
	if(channel >= 0 && channel < modules.Count()*8)
	{
		byte idx = floor(channel / 8);		
		if(idx < modules.Count())
		{
			RelayModuleInfo* info = modules.Get(idx);
			if (info)
			{
				byte bit = channel - (idx * 8);
				if (info->inverted)
					bit = 7 - bit;
				byte to_set = info->flags;
				if (info->activelow)
				{
					bitSet(to_set, bit);
				}
				else
				{
					bitClear(to_set, bit);
				}
				modules.Get(idx)->flags = to_set;
				update_pending = true;
			}
		}
		//WriteChannels();
	}
	return true;
}

bool CRelayModules::GetChannelStatus(byte channel)
{
	if (channel == 0xFF)
		return false;
	channel--;
	if(channel >= 0 && channel < modules.Count()*8)
	{
		byte idx = floor(channel / 8);		
		if(idx < modules.Count())
		{
			RelayModuleInfo* info = modules.Get(idx);
			if (info)
			{
				byte bit = channel - (idx * 8);
				if (info->inverted)
					bit = 7 - bit;
				bool value = (bool)bitRead(info->flags, bit);
				if (info->activelow)
				{
					return !value;
				}
				else
				{
					return value;
				}
			}
		}
	}
	return false;
}

String CRelayModules::GetInfoString()
{
	String result = "INFO: RELAYMODULE: ";
	if(modules.Count() == 0) return "INFO: RELAYMODULE: null";
	for(byte i = 1; i <= modules.Count()*8; i++)
	{
		result += " " + String((int)(i)) + ":";
		if(GetChannelStatus(i))
		{
			result += "ON";
		}
		else
		{
			result += "OFF";
		}
	}
	return result;
}

byte CRelayModules::GetFlags(byte module_idx)
{
	if (module_idx < modules.Count())
	{
		RelayModuleInfo* info = modules.Get(module_idx);
		return info->flags;
	}
}

void CRelayModules::PrintChannelAction(byte ch, bool open)
{
	StaticJsonBuffer<200> jBuff;
	JsonObject& root = jBuff.createObject();
	root[jKeyTarget] = jTargetRelay;
	root[jKeyAction] = open ? jRelayActionOpenChannel : jRelayActionCloseChannel;
	root[jKeyValue] = ch;
	PrintJson.Print(root);
}

void CRelayModules::OnTimeSlice()
{
	uint32_t t = millis();
	if (t - last_rewrite >= 60 * 1000)
	{
		update_pending = true;
	}
	if(update_pending)
	{
		last_rewrite = t;
		update_pending = false;
		Log.debug("RelayModuleClass::OnTimeSlice() - Writing channels.");
		WriteChannels();
	}
}

void CRelayModules::ParseJSON(JsonObject& jo)
{
	String action = jo[jKeyAction];
	if (action.equals(jKeyStatus))
	{
		StaticJsonBuffer<1024> jBuff;
		JsonObject& root = jBuff.createObject();
		root[jKeyTarget] = jTargetRelay;
		root[jKeyAction] = jKeyStatus;
		byte mcount = 8 * modules.Count();
		root["chcount"] = mcount;
		for (int i = 1; i <= mcount; i++)
		{
			root[String(i)] = GetChannelStatus(i);
		}
		PrintJson.Print(root);
		return;
	}
	if (action.equals(jRelayActionOpenChannel))
	{
		byte ch = jo[jKeyValue];
		OpenChannel(ch);
		return;
	}
	if (action.equals(jRelayActionCloseChannel))
	{
		byte ch = jo[jKeyValue];
		CloseChannel(ch);
		return;
	}
}

CRelayModules RelayModules;