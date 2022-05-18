#pragma once
#include <stdint.h>

const uint32_t MAX_ALARM_CHANNELS = 8;

struct AlarmChannelInfo
{
	uint8_t channel;
	uint8_t opened;
};

struct AlarmData
{
	uint8_t Valid;
	uint32_t alarm_temp;
	uint32_t histeresis;
	AlarmChannelInfo channelInfo[MAX_ALARM_CHANNELS];
};