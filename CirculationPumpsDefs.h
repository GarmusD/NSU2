#pragma once
#include <stdint.h>
#include "consts.h"

struct CircPumpData
{
	uint8_t Valid;
	char Name[MAX_NAME_LENGTH];
	char TempTriggerName[MAX_NAME_LENGTH];
	uint8_t MaxSpeed;
	uint8_t Spd1Channel;
	uint8_t Spd2Channel;
	uint8_t Spd3Channel;
};