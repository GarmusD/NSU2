#pragma once
#include <stdint.h>
#include "consts.h"

struct SwitchData
{
	uint8_t Valid;
	char Name[MAX_NAME_LENGTH];
	char DependOn[MAX_NAME_LENGTH];
	uint8_t depstate;
	uint8_t forcestate;
	uint8_t status;
};