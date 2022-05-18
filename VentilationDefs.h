#pragma once
#include <stdint.h>
#include "consts.h"

struct VentilationData
{
	uint8_t Valid;
	uint8_t Channel;
	char AirSensor[MAX_NAME_LENGTH];
	char RoomSensor[MAX_NAME_LENGTH];
	char InAirSensor[MAX_NAME_LENGTH];
};