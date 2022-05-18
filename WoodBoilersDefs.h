#pragma once
#include <stdint.h>
#include "consts.h"

struct WoodBoilerData
{
	uint8_t Valid;
	char Name[MAX_NAME_LENGTH];
	char TSensorName[MAX_NAME_LENGTH];
	char KTypeName[MAX_NAME_LENGTH];
	uint8_t LadomatChannel;
	uint8_t ExhaustFanChannel;
	int WorkingTemp;
	int WorkingHisteresis;
	int LadomatTemp; //<- nauja
	char LadomatTempTriggerName[MAX_NAME_LENGTH]; // <- nauja
	char WaterBoilerName[MAX_NAME_LENGTH]; // <- nauja
};