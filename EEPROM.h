#ifndef EEPROM_h
#define EEPROM_h

#include "Arduino.h"
#include "eeprom_consts.h"
#include "SdFat.h"
#include "Logger.h"
#include "FileManager.h"

class EEPROMClass
{
public:
	EEPROMClass(void);
	~EEPROMClass(void);
	bool Begin();
	byte GetVersion();

	bool GetBool(uint16_t pos);
	bool SetBool(uint16_t pos, bool value);

	byte GetByte(uint16_t pos);
	bool SetByte(uint16_t pos, byte value);

	

private:
	SdFile file;
	byte version;

	byte * virteeprom;
	bool GetVirtBool(uint16_t pos);
	bool SetVirtBool(uint16_t pos, bool value);

	byte GetVirtByte(uint16_t pos);
	bool SetVirtByte(uint16_t pos, byte value);

	void SetupVirtualEEPROM();
};

extern EEPROMClass EEPROM;

#endif