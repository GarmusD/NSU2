#pragma once
#include <stdint.h>
#include "consts.h"

enum ValveType
{
	NC,
	NO
};

struct ThermoValve
{
	ValveType type;
	uint8_t relay_channel;
};

struct CollectorData
{
	uint8_t Valid;
	char Name[MAX_NAME_LENGTH];
	char CircPump[MAX_NAME_LENGTH];
	uint8_t valve_count;
	ThermoValve valves[MAX_COLLECTOR_VALVES];
};