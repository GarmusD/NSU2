#ifndef nsu_pins_h
#define nsu_pins_h

const int SD_PIN = 53;

const int ONE_WIRE_PIN = 21;

//Pin connected to ST_CP of 74HC595
const int RM_LATCH_PIN = 16;//8;
const int RM_OE_PIN = 15;
//Pin connected to SH_CP of 74HC595
//const int RM_CLOCK_PIN = 15;//12;
////Pin connected to DS of 74HC595
//const int RM_DATA_PIN = 14;//11;

//MAX31855 PINS
const int MAX31855_CS = 17;//chip select

//TFT Backlight
const int LCDBacklightPin = 6;

#endif

