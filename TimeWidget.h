#ifndef TimeWidget_h
#define TimeWidget_h

#include "windows.h"
#include "UI.h"
#include "advancedFunctions.h"

class TimeWidget : public UILabel
{
public:
	TimeWidget(CWindow* window);
	~TimeWidget(void);

	void TimeEvent(uint32_t t);
private:
	unsigned long int lasttick;
	bool separator;
	int hour, min;
};

#endif
