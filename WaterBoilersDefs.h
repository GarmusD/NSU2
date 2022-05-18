#pragma once
#include <stdint.h>
#include "consts.h"

struct ElHeatingData
{
	uint8_t StartHour;
	uint8_t StartMin;
	uint8_t EndHour;
	uint8_t EndMin;
};

struct WaterBoilerData
{
	uint8_t Valid;
	char Name[MAX_NAME_LENGTH];
	char TSensorName[MAX_NAME_LENGTH];
	char TempTriggerName[MAX_NAME_LENGTH];
	char CircPumpName[MAX_NAME_LENGTH];
	uint8_t ElHeatingEnabled;
	uint8_t ElHeatingChannel;
	ElHeatingData HeatingData[7];
};