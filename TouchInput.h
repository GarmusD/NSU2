#ifndef TouchInput_h
#define TouchInput_h

#include <UTouch.h>
#include "msg.h"
#include "LCDBacklight.h"

class TouchInput
{
public:
	TouchInput(UTFT &_tft, UTouch &utouch);
	~TouchInput(void);
	bool MsgAvailable();
	Msg GetMessage();
	void ForceLCDBacklight();
private:
	MsgType lastmsgtype, msgtype;
	Msg msg;
	int x, y, lastx, lasty;
	UTouch &touch;
	LCDBacklight backlight;
	unsigned long int last_touch;
	byte tft_model;
	int tft_width;
};

#endif

