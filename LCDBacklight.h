#ifndef LCDBacklight_h
#define LCDBacklight_h

#include "Logger.h"
#include "nsu_pins.h"
#include "UTFT.h"
//#include "windows.h"

const uint32_t LCDWaitTime = 1000*60*15; //15 min
//const long int LCDWaitTime = 1000*60; //1 min
const unsigned long TransitionTime = 500;

class LCDBacklight
{
public:
	LCDBacklight(UTFT &_tft);
	~LCDBacklight(void);
	void TimeEvent(uint32_t t);
	bool LightIsOn(bool touched);
private:
	UTFT& tft;
	bool lightIsOn;
	bool inTransition;
	float increment;
	float current_pwm;
	uint32_t last_click;
	uint32_t transition_start;
	byte max_value, min_value;
	byte last_value;
	byte disp_model;
	void SetBLValue(byte value);
};

#endif
