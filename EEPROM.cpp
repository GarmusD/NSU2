#include "EEPROM.h"


EEPROMClass::EEPROMClass(void)
{
}


EEPROMClass::~EEPROMClass(void)
{
}

bool EEPROMClass::Begin()
{
	if(!FileManager.SDCardOk()){
		Log.error("EEPROM SD access error! Using Virtual EEPROM.");
		SetupVirtualEEPROM();
		return false;
	}
	virteeprom = NULL;
	if(!FileManager.SDFat().exists(EEPROM_FILE_NAME.c_str()))
	{
		Log.debug("EEPROM file does not exist. Creating.");
		if(file.open(EEPROM_FILE_NAME.c_str(), O_RDWR | O_CREAT))
		{
			for(uint32_t i = 0; i < EEPROM_SIZE; i++)
			{
				file.write((byte)0);
			}
			file.seekSet(0);
			file.write(EEPROM_VERSION);
			file.close();
		}
		else//file not created
		{
			return false;
		}
	}
	file.open(EEPROM_FILE_NAME.c_str(), O_RDWR);
	file.read(&version, 1);
	//checkVersion();
	file.close();
	Log.error("EEPROM Begin done OK.");
	return true;
}

void EEPROMClass::SetupVirtualEEPROM()
{
	Log.error("Setting up virtual EEPROM.");

	virteeprom = new byte[EEPROM_SIZE];
	
	SetByte(EEPROM_POS_SWITCH[SWITCH_NAME_WINTER_MODE], (byte)STATUS_ON);

	SetByte(EEPROM_POS_SWITCH[SWITCH_NAME_TEMPERATURE_MODE], (byte)STATUS_ON);

	SetByte(EEPROM_POS_SWITCH[SWITCH_NAME_HOT_WATER_MODE], (byte)STATUS_ON);

	SetByte(EEPROM_POS_SWITCH[SWITCH_NAME_BURN_MODE], (byte)STATUS_ON);
}

byte EEPROMClass::GetVersion()
{
	return version;
}

bool EEPROMClass::GetBool(uint16_t pos)
{
	if(!FileManager.SDCardOk()) return GetVirtBool(pos);
	if(file.open(EEPROM_FILE_NAME.c_str(), O_RDWR))
	{
		file.seekSet(pos);
		bool res = (bool)file.read();
		file.close();
		return res;
	}
	return false;
}

bool EEPROMClass::GetVirtBool(uint16_t pos)
{
	if (virteeprom == NULL) return false;
	return (bool)virteeprom[pos];
}

bool EEPROMClass::SetBool(uint16_t pos, bool value)
{
	if(!FileManager.SDCardOk()) return SetVirtBool(pos, value);
	if(file.open(EEPROM_FILE_NAME.c_str(), O_RDWR))
	{
		file.seekSet(pos);
		file.write((byte)value);
		file.sync();
		file.close();
		return true;
	}
	return false;
}

bool EEPROMClass::SetVirtBool(uint16_t pos, bool value)
{
	if (virteeprom == NULL) return false;
	virteeprom[pos] = (byte)value;
	return true;
}

byte EEPROMClass::GetByte(uint16_t pos)
{
	if(!FileManager.SDCardOk()) return GetVirtByte(pos);
	if(file.open(EEPROM_FILE_NAME.c_str(), O_RDWR))
	{
		file.seekSet(pos);
		byte res = file.read();
		file.close();
		return res;
	}
	return 0;
}

byte EEPROMClass::GetVirtByte(uint16_t pos)
{
	if (virteeprom == NULL) return 0;
	return virteeprom[pos];
}

bool EEPROMClass::SetByte(uint16_t pos, byte value)
{
	if(!FileManager.SDCardOk()) return SetVirtByte(pos, value);
	if(file.open(EEPROM_FILE_NAME.c_str(), O_RDWR))
	{
		file.seekSet(pos);
		file.write(value);
		file.sync();
		file.close();
		return true;
	}
	return false;
}

bool EEPROMClass::SetVirtByte(uint16_t pos, byte value)
{
	if (virteeprom == NULL) return false;
	virteeprom[pos] = value;
	return true;
}

EEPROMClass EEPROM;