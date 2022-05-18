#ifndef CONSTS_H
#define CONSTS_H

#include "Arduino.h"

const int SETTINGS_VERSION = 14;

const uint8_t MAX_NAME_LENGTH = 32;
const uint8_t MAX_UUID_LENGTH = 16;
const uint8_t MAX_CAPTION_LENGTH = 32;
const uint8_t MAX_LABEL_LENGTH = 128;

const uint8_t MAX_SWITCHES_COUNT = 16;
const uint8_t MAX_SENSOR_COUNT = 64;
const uint8_t MAX_RELAY_MODULES = 5;
const uint8_t MAX_TEMP_TRIGGERS = 16;
const uint8_t MAX_TEMP_TRIGGER_PIECES = 4;
const uint8_t MAX_CIRCULATION_PUMPS = 8;
const uint8_t MAX_COLLECTORS = 8;
const uint8_t MAX_COLLECTOR_VALVES = 16;
const uint8_t MAX_COMFORT_ZONES = 32;
const uint8_t MAX_KTYPE_SENSORS = 1;
const uint8_t MAX_WATERBOILER_COUNT = 1;
const uint8_t MAX_WATERBOILER_EL_HEATING_DATA_COUNT = 7;
const uint8_t MAX_WOOD_BOILERS_COUNT = 1;
const uint8_t MAX_SYSTEM_FAN_COUNT = 1;
const uint8_t MAX_ALARM_COUNT = 1;

const int FLASH_PIN = 52;

const uint8_t BUILT_IN_SWITCHES_COUNT = 4;
const uint8_t SWITCH_NAME_WINTER_MODE = 0;
const uint8_t SWITCH_NAME_TEMPERATURE_MODE = 1;
const uint8_t SWITCH_NAME_HOT_WATER_MODE = 2;
const uint8_t SWITCH_NAME_BURN_MODE = 3;
const char* const SWITCH_NAMES[BUILT_IN_SWITCHES_COUNT] = {"winter_mode", "temp_mode", "boiler_mode", "burn_mode"};

const float MAX_BAR_TEMP = 70.0;
const float MIN_BAR_TEMP = 32.0;

const uint32_t MINUTES1 = 1000 * 60;
const uint32_t MINUTES5 = 1000 * 60 * 5;
const uint32_t MINUTES10 = MINUTES5 * 2;
const uint32_t MINUTES15 = MINUTES5 * 3;
const uint32_t MINUTES60 = MINUTES1 * 60;

#endif