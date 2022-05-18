#pragma once
#include <stdint.h>
#include "consts.h"

struct KTypeData
{
	uint8_t Valid;
	char Name[MAX_NAME_LENGTH];
	int interval;
};