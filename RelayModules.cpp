#include <Arduino.h>
#include "RelayModules.h"
#include "nsu_pins.h"
#include <SPI.h>
#include "SystemStatus.h"

const int CRelayModules::CURRENT_CONFIG_VERSION = 2;
const VersionInfo CRelayModules::VINFO = { 'R', 'L', 'M', CURRENT_CONFIG_VERSION };
const char* CRelayModules::CFG_FILE = "relays.cfg";

void RelayModuleInfo::Reset()
{
	activelow = true;
	inverted = false;
	flags = 0;
	lock = 0;
	uflags = 0;
}

//RelayModule* RelayModule::_instance = NULL;

CRelayModules::CRelayModules(void)
{	
	update_pending = false;
	last_rewrite = 0;
}

CRelayModules::~CRelayModules(void)
{
}

void CRelayModules::InitHW()
{
	//pinMode(RM_OE_PIN, INPUT);
	pinMode(RM_OE_PIN, OUTPUT);
	digitalWrite(RM_OE_PIN, HIGH);

	//pinMode(RM_LATCH_PIN, INPUT);
	pinMode(RM_LATCH_PIN, OUTPUT);
	digitalWrite(RM_LATCH_PIN, HIGH);
}

void CRelayModules::Begin()
{
	TimeSlice.RegisterTimeSlice(this);
}

void CRelayModules::Reset()
{
	modules.Clear();
	update_pending = false;
	last_rewrite = 0;
}


void CRelayModules::AddRelayModule(uint8_t activelow, uint8_t inverted)
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
	SPI.beginTransaction(SPISettings());
	digitalWrite(RM_OE_PIN, HIGH);
	digitalWrite(RM_LATCH_PIN, LOW);
	for(uint8_t i = 0; i < modules.Count(); i++)
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
	digitalWrite(RM_OE_PIN, LOW);
	SPI.endTransaction();
	Log.debug("RelayModuleClass::AllChannelsOff() done.");
}

void CRelayModules::WriteChannels()
{
	SPI.beginTransaction(SPISettings());
	digitalWrite(RM_OE_PIN, HIGH);
	digitalWrite(RM_LATCH_PIN, LOW);
	int cnt = modules.Count();
	for(int i = cnt-1; i >= 0; --i)
	{
		RelayModuleInfo* rmi = modules.Get(i);
		if(rmi != NULL)
		{
			SPI.transfer(rmi->flags);
		}
	}
	digitalWrite(RM_LATCH_PIN, HIGH);
	digitalWrite(RM_OE_PIN, LOW);
	SPI.endTransaction();
	//Debug
	for (int i = cnt - 1; i >= 0; --i)
	{
		RelayModuleInfo* rmi = modules.Get(i);
		if (rmi != NULL)
		{
			char s[128];
			sprintf(s, "RelayModule %d writen flags: %d", i, rmi->flags);
			Log.debug(s);
		}
	}
}

bool CRelayModules::OpenChannel(uint8_t channel)
{
	if (channel == 0xFF)
		return false;	
	channel--;
	if(channel >= 0 && channel < modules.Count()*8)
	{
		uint8_t idx = floor(channel / 8);		
		if(idx < modules.Count())
		{
			RelayModuleInfo* info = modules.Get(idx);
			if (info)
			{
				uint8_t bit = channel - (idx * 8);
				if (info->inverted)
					bit = 7 - bit;
				if (bitRead(info->lock, bit))
				{
					bitSet(info->uflags, bit);
					char s[64];
					sprintf(s, "Channel %d is locked by alarm.", channel+1);
					Log.debug(s);
					return false;
				}
				if (info->activelow)
				{
					bitClear(info->flags, bit);
				}
				else
				{
					bitSet(info->flags, bit);
				}
				update_pending = true;
				PrintChannelAction(channel+1, true);
			}
		}
		//WriteChannels();
	}
	return true;
}

bool CRelayModules::CloseChannel(uint8_t channel)
{
	if (channel == 0xFF)
		return false;
	PrintChannelAction(channel, false);
	channel--;
	if(channel >= 0 && channel < modules.Count()*8)
	{
		uint8_t idx = floor(channel / 8);		
		if(idx < modules.Count())
		{
			RelayModuleInfo* info = modules.Get(idx);
			if (info)
			{
				uint8_t bit = channel - (idx * 8);
				if (info->inverted)
					bit = 7 - bit;
				if (bitRead(info->lock, bit))
				{
					bitClear(info->uflags, bit);
					char s[64];
					sprintf(s, "Channel %d is locked by alarm.", channel + 1);
					Log.debug(s);
					return false;
				}				
				if (info->activelow)
				{
					bitSet(info->flags, bit);
				}
				else
				{
					bitClear(info->flags, bit);
				}
				update_pending = true;
			}
		}
		//WriteChannels();
	}
	return true;
}

void CRelayModules::LockChannel(uint8_t ch, bool value)
{
	if (ch == 0xFF)
		return;	
	ch--;
	if (ch >= 0 && ch < modules.Count() * 8)
	{
		uint8_t idx = floor(ch / 8);
		if (idx < modules.Count())
		{
			RelayModuleInfo* info = modules.Get(idx);
			if (info)
			{
				uint8_t bit = ch - (idx * 8);
				if (info->inverted)
					bit = 7 - bit;
				if (bitRead(info->lock, bit) == 0)
				{
					bitSet(info->lock, bit);
					bitRead(info->flags, bit) != 0 ? bitSet(info->uflags, bit) : bitClear(info->uflags, bit);
					if (value)
					{
						if (info->activelow)
						{
							bitClear(info->flags, bit);
						}
						else
						{
							bitSet(info->flags, bit);
						}
					}
					else
					{
						if (info->activelow)
						{
							bitSet(info->flags, bit);
						}
						else
						{
							bitClear(info->flags, bit);
						}
					}
					update_pending = true;
					char s[64];
					sprintf(s, "RelayModules: LockChannel(%d);", ch+1);
					Log.info(s);
					PrintChannelAction(ch+1, value);
				}
			}
		}
	}
}

void CRelayModules::UnlockChannel(uint8_t ch)
{
	if (ch == 0xFF)
		return;
	ch--;
	if (ch >= 0 && ch < modules.Count() * 8)
	{
		uint8_t idx = floor(ch / 8);
		if (idx < modules.Count())
		{
			RelayModuleInfo* info = modules.Get(idx);
			if (info)
			{
				uint8_t bit = ch - (idx * 8);
				if (info->inverted)
					bit = 7 - bit;
				//uint8_t to_set = info->flags;
				if (bitRead(info->lock, bit))
				{
					bitClear(info->lock, bit);
					uint8_t value = bitRead(info->uflags, bit);
					if (value)
					{
						if (info->activelow)
						{
							bitClear(info->flags, bit);
						}
						else
						{
							bitSet(info->flags, bit);
						}
					}
					else
					{
						if (info->activelow)
						{
							bitSet(info->flags, bit);
						}
						else
						{
							bitClear(info->flags, bit);
						}
					}
					update_pending = true;
					char s[64];
					sprintf(s, "RelayModules: UnlockChannel(%d);", ch + 1);
					Log.info(s);
					PrintChannelAction(ch + 1, value);
				}
			}
		}
	}
}

bool CRelayModules::GetChannelStatus(uint8_t channel)
{
	if (channel == 0xFF)
		return false;
	channel--;
	if(channel >= 0 && channel < modules.Count()*8)
	{
		uint8_t idx = floor(channel / 8);		
		if(idx < modules.Count())
		{
			RelayModuleInfo* info = modules.Get(idx);
			if (info)
			{
				uint8_t bit = channel - (idx * 8);
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

uint8_t CRelayModules::GetFlags(uint8_t module_idx)
{
	if (module_idx < modules.Count())
	{
		RelayModuleInfo* info = modules.Get(module_idx);
		return info->flags;
	}
}

void CRelayModules::PrintChannelAction(uint8_t ch, bool open, bool locked)
{
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
	root[jKeyTarget] = jTargetRelay;
	root[jKeyAction] = open ? jRelayActionOpenChannel : jRelayActionCloseChannel;
	root[jKeyValue] = ch;
	root[jRelayChannelLocked] = locked;
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

void CRelayModules::LoadConfig()
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_RELAY_MODULES; i++)
		{
			RelayModuleData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					AddRelayModule(data.ActiveLow, data.Inverted);
				}
			}
			else
			{
				//Read error
				char s[64];
				sprintf(s, "RelayModule config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_RELAY_MODULES; i++)
		{
			const RelayModuleData& data = Defaults.GetDefaultRelayModuleData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					AddRelayModule(data.ActiveLow, data.Inverted);
				}
			}
			else
			{
				char s[64];
				sprintf(s, "RelayModule config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_RELAY_MODULES; i++)
		{
			const RelayModuleData& data = Defaults.GetDefaultRelayModuleData(i);
			if (data.Valid)
			{
				AddRelayModule(data.ActiveLow, data.Inverted);
			}
		}
		break;
	default:
		break;
	}
}

bool CRelayModules::GetConfigData(uint8_t cfgPos, RelayModuleData & data)
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

bool CRelayModules::ValidateSetupDataSet(JsonObject & jo)
{
	if (jo.containsKey(jKeyConfigPos) &&
		jo.containsKey(jKeyEnabled) &&
		jo.containsKey(jRelayActiveLow) &&
		jo.containsKey(jRelayInverted)
		)
	{
		return true;
	}
	PrintJson.PrintResultError(jTargetRelay, jErrorInvalidDataSet);
	return false;
}

void CRelayModules::ParseJSON(JsonObject& jo)
{
	const char * action = jo[jKeyAction];
	if (strcmp(action, jKeyStatus) == 0)
	{
		DynamicJsonDocument jBuff(1024);
		JsonObject root = jBuff.to<JsonObject>();
		root[jKeyTarget] = jTargetRelay;
		root[jKeyAction] = jKeyStatus;
		uint8_t mcount = 8 * modules.Count();
		root["chcount"] = mcount;
		char s[12];
		for (int i = 1; i <= mcount; i++)
		{
			sprintf(s, "%d", i);
			root[s] = GetChannelStatus(i);
		}
		PrintJson.Print(root);
		return;
	}
	else if (strcmp(action, jRelayActionOpenChannel) == 0)
	{
		uint8_t ch = jo[jKeyValue];
		OpenChannel(ch);
		return;
	}
	else if (strcmp(action, jRelayActionCloseChannel) == 0)
	{
		uint8_t ch = jo[jKeyValue];
		CloseChannel(ch);
		return;
	}
	else if (strcmp(action, jRelayActionLockChannel) == 0)
	{
		uint8_t ch = jo[jRelayChannel];
		bool open = jo[jKeyValue];
		LockChannel(ch, open);
		return;
	}
	else if (strcmp(action, jRelayActionUnlockChannel) == 0)
	{
		uint8_t ch = jo[jRelayChannel];
		UnlockChannel(ch);
		return;
	}
	else if (strcmp(action, jKeySetup) == 0)
	{
		if (ValidateSetupDataSet(jo))
		{
			RelayModuleData data;
			data.Valid = jo[jKeyEnabled];
			data.ActiveLow = jo[jRelayActiveLow];
			data.Inverted = jo[jRelayInverted];
			uint8_t cfgPos = jo[jKeyConfigPos];
			if (FileManager.OpenConfigFile(CFG_FILE, VINFO))
			{
				if (FileManager.FileSeek(sizeof(VersionInfo) + cfgPos * sizeof(data)))
				{
					if (FileManager.FileWriteBuff(&data, sizeof(data)))
					{
						FileManager.FileClose();
						SystemStatus.SetRebootRequired();
						PrintJson.PrintResultOk(jTargetRelay, action, true);
					}
				}
			}
			PrintJson.PrintResultError(jTargetRelay, jErrorFileOpenError);
		}
	}
	else
	{
		PrintJson.PrintResultUnknownAction(jTargetRelay, action);
	}
}

CRelayModules RelayModules;
