#ifndef SideWindow_h
#define SideWindow_h

#include "Logger.h"
#include "TimeWidget.h"
#include "UI.h"
#include "List.h"
#include "graphics.h"
#include "Window.h"

const int BTN_SPACE = 5;

//typedef struct BtnInfo
//{
//	int bntH;
//	ImgButton* btn
//};

class SideWindow : public CWindow
{
public:
	SideWindow(/*Rect windowarea, */UTFT& utft, Scenario& Sc, uint8_t* sc_buf);
	~SideWindow(void);

	void AddButton(UIImgButton* btn);
	void Begin(Rect& area);
private:
	TimeWidget timewidget;
	UISwitchButton btnOpMode;
	UISwitchButton btnTempMode;
	UISwitchButton btnKarstasVanduo;

	List<UIImgButton> buttons;
};

#endif
