#ifndef CONSTS_H
#define CONSTS_H

#include "Arduino.h"

const int SETTINGS_VERSION = 11;

const byte MAX_NAME_LENGTH = 32;
const byte MAX_UUID_LENGTH = 16;
const byte MAX_CAPTION_LENGTH = 32;
const byte MAX_LABEL_LENGTH = 128;

const int MAX_SWITCHES_COUNT = 16;

const byte MAX_WOOD_BOILERS_COUNT = 1;

const int FLASH_PIN = 52;

const char CONFIG_FILE[] = "config.nsu";
const char CONFIG_FILE_OLD[] = "configOLD.nsu";

const byte BUILT_IN_SWITCHES_COUNT = 4;
const byte SWITCH_NAME_WINTER_MODE = 0;
const byte SWITCH_NAME_TEMPERATURE_MODE = 1;
const byte SWITCH_NAME_HOT_WATER_MODE = 2;
const byte SWITCH_NAME_BURN_MODE = 3;
const String SWITCH_NAMES[BUILT_IN_SWITCHES_COUNT] = {"winter_mode", "temp_mode", "boiler_mode", "burn_mode"};

const int MAX_BAR_TEMP = 70.0;
const int MIN_BAR_TEMP = 32.0;

const uint32_t MINUTES1 = 1000 * 60;
const uint32_t MINUTES5 = 1000 * 60 * 5;
const uint32_t MINUTES10 = MINUTES5 * 2;
const uint32_t MINUTES15 = MINUTES5 * 3;
const uint32_t MINUTES60 = MINUTES1 * 60;

#endif