#include "TouchInput.h"
//#include <UTouch.h>

TouchInput::TouchInput(UTFT &tft, UTouch &utouch):backlight(tft), touch(utouch)
{
	//touch = new UTouch(6,5,32,3,2);
	touch.InitTouch();
	touch.setPrecision(PREC_HI);
	lastmsgtype = NoTouch;
	last_touch = millis();
	tft_model = CPLD;// tft->display_model;
	tft_width = 799;//tft->disp_x_size;
}

//int x = 0;
TouchInput::~TouchInput(void)
{
}

void TouchInput::ForceLCDBacklight()
{
	backlight.LightIsOn(true);
}

bool TouchInput::MsgAvailable()
{
	//x++;
	//if( (x % 1000) == 0) Log.debug("TouchInput::MsgAvailable()");
	bool data_avail = touch.dataAvailable();
	if(!backlight.LightIsOn(data_avail))
	{
		if(data_avail){
			touch.read();
			x = touch.getX();
			y = touch.getY();
		}
		return false;
	}
	if(data_avail && millis()-last_touch > 50)
	{
		//t1 = millis();
		//Serial.println("MsgAvailable");
		touch.read();
		x = touch.getX();
		if(tft_model == CPLD)
		{
			x = tft_width - x;
		}
		y = touch.getY();
		Log.debug("Touch X: "+String(x)+", Y: "+String(y));

		if(x == -1 || y == -1)
		{
			x = lastx;
			y = lasty;
		}
		switch (lastmsgtype)
		{
		case NoTouch:
			msg.type = TouchDown;
			msg.x = x;
			msg.y = y;
			lastmsgtype = TouchDown;
			break;
		case TouchDown:
			msg.type = TouchMove;
			msg.x = x;
			msg.y = y;
			lastmsgtype = TouchMove;
			break;
		case TouchMove:
			msg.type = TouchMove;
			msg.x = x;
			msg.y = y;
			lastmsgtype = TouchMove;
			break;
		default:
			break;
		}
		lastx = x;
		lasty = y;
		return true;
	}else if(lastmsgtype == TouchMove || lastmsgtype == TouchDown)
	{
		msg.type = TouchUp;
		msg.x = lastx;
		msg.y = lasty;
		lastmsgtype = TouchUp;
		last_touch = millis();
		return true;
	}else if(lastmsgtype == TouchUp)
	{
		lastmsgtype = NoTouch;
	}
	return false;
}

Msg TouchInput::GetMessage()
{
	return msg;
}

