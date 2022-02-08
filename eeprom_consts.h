#ifndef eeprom_consts_h
#define eeprom_consts_h

#include "consts.h"
#include "switches.h"
#include "ComfortZones.h"

const String EEPROM_FILE_NAME = "eeprom";

const uint16_t EEPROM_SIZE = 256;
//Positions in "EEPROM"
const uint16_t EEPROM_VERSION = 1;

const uint16_t EEPROM_POS_WBOILER_POWER_ENABLED = 5;

const uint16_t EEPROM_SWITCH_START = 10;
const uint16_t EEPROM_POS_SWITCH[MAX_SWITCHES_COUNT] = {EEPROM_SWITCH_START + 0,
														EEPROM_SWITCH_START + 1,
														EEPROM_SWITCH_START + 2,
														EEPROM_SWITCH_START + 3,
														EEPROM_SWITCH_START + 4,
														EEPROM_SWITCH_START + 5,
														EEPROM_SWITCH_START + 6,
														EEPROM_SWITCH_START + 7,
														EEPROM_SWITCH_START + 8,
														EEPROM_SWITCH_START + 9,
														EEPROM_SWITCH_START + 10,
														EEPROM_SWITCH_START + 11,
														EEPROM_SWITCH_START + 12,
														EEPROM_SWITCH_START + 13,
														EEPROM_SWITCH_START + 14,
														EEPROM_SWITCH_START + 15};

const uint16_t EEPROM_COMFORT_ZONE_ROOM_START = EEPROM_SWITCH_START + MAX_SWITCHES_COUNT;
const uint16_t EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[MAX_COMFORT_ZONES] = {
	EEPROM_COMFORT_ZONE_ROOM_START + 0,
	EEPROM_COMFORT_ZONE_ROOM_START + 1,
	EEPROM_COMFORT_ZONE_ROOM_START + 2,
	EEPROM_COMFORT_ZONE_ROOM_START + 3,
	EEPROM_COMFORT_ZONE_ROOM_START + 4,
	EEPROM_COMFORT_ZONE_ROOM_START + 5,
	EEPROM_COMFORT_ZONE_ROOM_START + 6,
	EEPROM_COMFORT_ZONE_ROOM_START + 7,
	EEPROM_COMFORT_ZONE_ROOM_START + 8,
	EEPROM_COMFORT_ZONE_ROOM_START + 9,
	EEPROM_COMFORT_ZONE_ROOM_START + 10,
	EEPROM_COMFORT_ZONE_ROOM_START + 11,
	EEPROM_COMFORT_ZONE_ROOM_START + 12,
	EEPROM_COMFORT_ZONE_ROOM_START + 13,
	EEPROM_COMFORT_ZONE_ROOM_START + 14,
	EEPROM_COMFORT_ZONE_ROOM_START + 15,
	EEPROM_COMFORT_ZONE_ROOM_START + 16,
	EEPROM_COMFORT_ZONE_ROOM_START + 17,
	EEPROM_COMFORT_ZONE_ROOM_START + 18,
	EEPROM_COMFORT_ZONE_ROOM_START + 19,
	EEPROM_COMFORT_ZONE_ROOM_START + 20,
	EEPROM_COMFORT_ZONE_ROOM_START + 21,
	EEPROM_COMFORT_ZONE_ROOM_START + 22,
	EEPROM_COMFORT_ZONE_ROOM_START + 23,
	EEPROM_COMFORT_ZONE_ROOM_START + 24,
	EEPROM_COMFORT_ZONE_ROOM_START + 25,
	EEPROM_COMFORT_ZONE_ROOM_START + 26,
	EEPROM_COMFORT_ZONE_ROOM_START + 27,
	EEPROM_COMFORT_ZONE_ROOM_START + 28,
	EEPROM_COMFORT_ZONE_ROOM_START + 29,
	EEPROM_COMFORT_ZONE_ROOM_START + 30,
	EEPROM_COMFORT_ZONE_ROOM_START + 31
};

const uint16_t EEPROM_COMFORT_ZONE_FLOOR_START = EEPROM_COMFORT_ZONE_ROOM_START + MAX_COMFORT_ZONES;
const uint16_t EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[MAX_COMFORT_ZONES] = {
	EEPROM_COMFORT_ZONE_FLOOR_START + 0,
	EEPROM_COMFORT_ZONE_FLOOR_START + 1,
	EEPROM_COMFORT_ZONE_FLOOR_START + 2,
	EEPROM_COMFORT_ZONE_FLOOR_START + 3,
	EEPROM_COMFORT_ZONE_FLOOR_START + 4,
	EEPROM_COMFORT_ZONE_FLOOR_START + 5,
	EEPROM_COMFORT_ZONE_FLOOR_START + 6,
	EEPROM_COMFORT_ZONE_FLOOR_START + 7,
	EEPROM_COMFORT_ZONE_FLOOR_START + 8,
	EEPROM_COMFORT_ZONE_FLOOR_START + 9,
	EEPROM_COMFORT_ZONE_FLOOR_START + 10,
	EEPROM_COMFORT_ZONE_FLOOR_START + 11,
	EEPROM_COMFORT_ZONE_FLOOR_START + 12,
	EEPROM_COMFORT_ZONE_FLOOR_START + 13,
	EEPROM_COMFORT_ZONE_FLOOR_START + 14,
	EEPROM_COMFORT_ZONE_FLOOR_START + 15,
	EEPROM_COMFORT_ZONE_FLOOR_START + 16,
	EEPROM_COMFORT_ZONE_FLOOR_START + 17,
	EEPROM_COMFORT_ZONE_FLOOR_START + 18,
	EEPROM_COMFORT_ZONE_FLOOR_START + 19,
	EEPROM_COMFORT_ZONE_FLOOR_START + 20,
	EEPROM_COMFORT_ZONE_FLOOR_START + 21,
	EEPROM_COMFORT_ZONE_FLOOR_START + 22,
	EEPROM_COMFORT_ZONE_FLOOR_START + 23,
	EEPROM_COMFORT_ZONE_FLOOR_START + 24,
	EEPROM_COMFORT_ZONE_FLOOR_START + 25,
	EEPROM_COMFORT_ZONE_FLOOR_START + 26,
	EEPROM_COMFORT_ZONE_FLOOR_START + 27,
	EEPROM_COMFORT_ZONE_FLOOR_START + 28,
	EEPROM_COMFORT_ZONE_FLOOR_START + 29,
	EEPROM_COMFORT_ZONE_FLOOR_START + 30,
	EEPROM_COMFORT_ZONE_FLOOR_START + 31
};


#endif