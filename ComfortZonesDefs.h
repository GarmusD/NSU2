#pragma once
#include <stdint.h>
#include "consts.h"

struct ComfortZoneData
{
	uint8_t Valid;
	char Name[MAX_NAME_LENGTH];
	char Title[MAX_NAME_LENGTH];
	char room_sensor[MAX_NAME_LENGTH];
	char floor_sensor[MAX_NAME_LENGTH];
	char col_name[MAX_NAME_LENGTH];
	int room_temp_hi;
	int room_temp_low; //
	int floor_temp_hi; //
	int floor_temp_low; //
	int histeresis;
	uint8_t actuator;
	uint8_t low_temp_mode;
};