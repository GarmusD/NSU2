#pragma once
#include <stdint.h>
#include "consts.h"

enum TriggerCondition
{
	TrueIfLower,
	TrueIfHigher
};

struct TempTriggerPieceData
{
	uint8_t Valid;
	char SensorName[MAX_NAME_LENGTH];
	uint8_t Condition;
	int Temperature;
	int Histeresis;
};

struct TempTriggerData
{
	uint8_t Valid;
	char Name[MAX_NAME_LENGTH];
	TempTriggerPieceData ttpiece[4];
};