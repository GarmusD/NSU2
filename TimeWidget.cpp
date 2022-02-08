#include "TimeWidget.h"
#include "advancedFunctions.h"
#include "UI.h"

TimeWidget::TimeWidget(CWindow* window):UILabel(window)
{
	lasttick = 0;
	separator = false;
}


TimeWidget::~TimeWidget(void)
{
}

void TimeWidget::TimeEvent(uint32_t t){
	String stime;
	if(t - lasttick >= 500){
		lasttick = t;
		separator = !separator;
		hour = rtc.getHours();
		min = rtc.getMinutes();

		if(hour < 10) stime += "0";
		stime += String(hour);
		if(separator){
			stime += ":";
		}else{
			stime += " ";
		}
		if(min < 10) stime += "0";
		stime += String(min);

		SetCaption(stime);
		Invalidate();
	}
}