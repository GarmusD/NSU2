#pragma once
#include <stdint.h>
#include "consts.h"
#include <DallasTemperature.h>

struct SensorID {
	DeviceAddress addr;
	char name[MAX_NAME_LENGTH];
};

struct TSensorData
{
	uint8_t Valid;
	SensorID sid;
	int interval;
};