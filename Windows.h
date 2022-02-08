#ifndef windows_h
#define windows_h

#include "win_defs.h"
#include "fonts.h"
#include <UTFT.h>
//#include <UTouch.h>
/*********************************************
*********************************************/
#include "Window.h"
#include "Events.h"
#include "UITheme.h"
#include "TouchInput.h"
#include "msg.h"
#include "Scenario.h"
#include "List.h"
/*********************************************
*********************************************/

//#define FLASH_CS_PIN 52
//#define SPI_RATE 21//4

//Font selection 
//BVS= Bitstream vera sans, suffix = font size in pixel (height)
//#define BVS_13 10
//#define BVS_15 12
//#define BVS_19 14
//#define BVS_22 18
//#define BVS_28 22
//#define BVS_34 28
//#define BVS_43 38
//#define BVS_52 53
//#define BVS_74 78
//#define BVS_112 122

/*********************************************
*********************************************/

//class WindowBase
//{
//public:
//	WindowBase(UTFT* utft);
//	~WindowBase(void);
//private:
//	UTFT* tft;
//}
/*********************************************
*********************************************/

class CWindowInfo
{
public:
	CWindowInfo(CWindow* win, String name);
	CWindow* Window;
	String Name;
	bool Active;
};


/*********************************************
*********************************************/
class CWindows:public Events
{
public:
	CWindows(UTFT &utft, UTouch &utouch);
	~CWindows(void);
	void Begin();
	UTFT& getUTFT();
	//UTFT_CTE* getUTFT_CTE();
	Rect getAvailableArea();
	Rect getSideWindowArea();
	Scenario& getScenario();
	//void TimeEvent(uint32_t t);
	void DrawCurrentWindow();
	//void CreateSideWindow();
	void CloseWindow(CWindow* win);
	void ClearScreen();
	CWindow* CreateWindow();
	CWindow* AddWindow(CWindow* createdWindow);
	void AddSideWindow(CWindow* swindow);
	CWindow* CreateWindow(byte* sc_buf);
	CWindow* CreateModal();
	void ReInitTFT();
	void ForceLCDBacklight();
private:
	UTFT &tft;
	UTouch &touch;
	TouchInput* touchinput;
	Scenario* scenario;
	
	CWindow* sideWindow;
	CWindow* currentWindow;
	CWindow* previousWindow;
	CWindow* modalWindow;

	Rect availableArea;
	bool sw_enabled;
	bool sw_area_calculated;
	Rect sw_area;
	uint32_t last_reinit;
	
	void DisableSideWindow();
	void EnableSideWindow();
	unsigned char ReadFontIC_ID();
	void OnTimeSlice();
	List<CWindowInfo> allWindows;
	List<CWindow> sideWindows;
};

extern CWindows Windows;

#endif


