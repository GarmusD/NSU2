#include "LCDBacklight.h"

LCDBacklight::LCDBacklight(UTFT &_tft):tft(_tft)
{
	Log.debug("Creating LCDBacklight...");
	disp_model = tft.display_model;
	switch (disp_model)
	{
	case CPLD:
		max_value = 16;
		min_value = 0;
		break;
	case CTE70:
		max_value = 255;
		min_value = 0;
		pinMode(LCDBacklightPin, OUTPUT);
		digitalWrite(LCDBacklightPin, HIGH);
		//analogWrite(LCDBacklightPin, 255);
		break;
	default:
		break;
	}
	//analogWriteResolution(8);
	
	lightIsOn = true;
	inTransition = false;
	increment = (float)max_value / (float)TransitionTime;
	current_pwm = (float)max_value;
	last_value = max_value;
	last_click = millis();
}


LCDBacklight::~LCDBacklight(void)
{
}

void LCDBacklight::SetBLValue(uint8_t value)
{
	switch (disp_model)
	{
	case CPLD:
		tft.setBrightness( value );
		break;
	case CTE70:
		//analogWrite(LCDBacklightPin, value);
		break;
	default:
		break;
	}
	
}

void LCDBacklight::TimeEvent(uint32_t t)
{
	if(inTransition)
	{
		unsigned long diff = millis() - transition_start;
		uint8_t br_value;
		if(lightIsOn)
		{
			current_pwm = max_value - (increment * diff);
			if(current_pwm <= min_value)
			{
				current_pwm = min_value;
				inTransition = false;
				lightIsOn = false;
			}
			//analogWrite(LCDBacklightPin, floor(current_pwm + 0.5));
			br_value = (uint8_t)floor(current_pwm + 0.5);
			if(last_value != br_value){
				SetBLValue(br_value);
				last_value = br_value;
			}
		}
		else
		{
			current_pwm = min_value + (increment * diff);
			if(current_pwm >= max_value) 
			{
				current_pwm = max_value;
				inTransition = false;
				lightIsOn = true;
			}
			//analogWrite(LCDBacklightPin, floor(current_pwm + 0.5));
			br_value = (uint8_t)floor(current_pwm + 0.5);
			if(last_value != br_value)
			{
				SetBLValue(br_value);
				last_value = br_value;
			}
		}
	}
	else if(lightIsOn)
	{
		if((millis() - last_click) > (LCDWaitTime) && !inTransition)
		{
			char s[128];
			sprintf(s, "Changing LCD transaction... Increment: %.3f", increment);
			Log.debug(s);
			inTransition = true;
			transition_start = millis();
		}
	}
}

bool LCDBacklight::LightIsOn(bool touched)
{
	//return true;
	TimeEvent(0);
	if(inTransition){
		return false;
	}
	if(touched){
		last_click = millis();
		if(!inTransition && !lightIsOn){
			inTransition = true;
			transition_start = millis();
		}
	}
	return lightIsOn;
}
