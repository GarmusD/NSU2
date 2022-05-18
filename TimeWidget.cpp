#include "TimeWidget.h"
#include "advancedFunctions.h"
#include "UI.h"

TimeWidget::TimeWidget(CWindow* window):UILabel(window)
{
	lasttick = 0;
	separator = false;
	r = 0;
}


TimeWidget::~TimeWidget(void)
{
}

void TimeWidget::TimeEvent(uint32_t t)
{
	if(t - lasttick >= 500)
	{
		lasttick = t;
		if (SystemStatus.TimeIsSet())
		{
			separator = !separator;
			hour = rtc.getHours();
			min = rtc.getMinutes();

			sprintf(buff, "%02d%s%02d", hour, separator ? ":" : " ", min);
			SetCaption(buff);
			//Invalidate();
		}
		else
		{
			switch (r)
			{
			case 0:
				SetCaption("--:--");
				break;
			case 1:
				SetCaption("\\\\:\\\\");
				break;
			case 2:
				SetCaption("||:||");
				break;
			case 3:
				SetCaption("//://");
				break;
			default:
				break;
			}
			r++;
			if (r >= 3) r = 0;
		}
	}
}