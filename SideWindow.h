#ifndef SideWindow_h
#define SideWindow_h

#include "Logger.h"
#include "TimeWidget.h"
#include "UI.h"
#include "List.h"
#include "graphics.h"

const int BTN_SPACE = 5;

//typedef struct BtnInfo
//{
//	int bntH;
//	ImgButton* btn
//};

class SideWindow : public CWindow
{
public:
	SideWindow(Rect windowarea, UTFT& utft, Scenario& Sc, byte* sc_buf);
	~SideWindow(void);

	void AddButton(UIImgButton* btn);

	//void TimeEvent(uint32_t t);
private:
	TimeWidget* timewidget;
	List<UIImgButton> buttons;
};

#endif
