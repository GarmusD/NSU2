#pragma once
#include <stdint.h>
#include "consts.h"

struct SystemFanData
{
	uint8_t Valid;
	char Name[MAX_NAME_LENGTH];
	char TSensorName[MAX_NAME_LENGTH];
	int MinTemp;
	int MaxTemp;
};