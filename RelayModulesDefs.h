#pragma once
#include <stdint.h>
#include "consts.h"

struct RelayModuleData
{
	uint8_t Valid;
	uint8_t ActiveLow;
	uint8_t Inverted;
};