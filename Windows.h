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
#include "WindowSplash.h"
#include "SideWindow.h"
#include "WindowKatiline.h"
/*********************************************
*********************************************/

enum CurrentWindow
{
	CurrWindowNone,
	CurrWindowsSplash,
	CurrWindowMain
};

/*********************************************
*********************************************/
class CWindows:public Events
{
public:
	CWindows(UTFT &utft, UTouch &utouch);
	~CWindows(void);
	void Begin(void);
	UTFT& getUTFT(void);
		
	Scenario& getScenario(void);
	
	Splash & GetSplashScreen(void);
	WindowKatiline & GetMainWindow(void);
	SideWindow & GetSideWindow(void);

	Rect& GetSplashArea(void);
	Rect& GetSideWindowArea(void);
	Rect& GetMainWindowArea(void);

	void DrawSplashScreen(void);
	void DrawMainWindow(void);
	void DrawSideWindow(void);

	void ClearScreen(void);
	
	void ReInitTFT(void);
	void ForceLCDBacklight(void);
	void Reset(void);
	void InitHW(void);
	void BeginSplashWindow(void);
	void BeginSideWindow(void);
	void BeginMainWindow(void);
private:
	UTFT &tft;
	UTouch &touch;
	TouchInput touchinput;
	Scenario scenario;
	//TouchInput* touchinput;
	//Scenario* scenario;
	
	SideWindow sideWindow;
	WindowKatiline mainWindow;
	Splash splashWindow;

	CurrentWindow currWindow;

	Rect availableArea;
	Rect side_area;
	Rect main_area;
	uint32_t last_reinit;
	
	unsigned char ReadFontIC_ID(void);

	

	void OnTimeSlice();
};

extern CWindows Windows;

#endif


