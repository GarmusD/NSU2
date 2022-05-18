#include "windows.h"
#include "win_defs.h"
#include "UI.h"
#include <UTouch.h>
#include "SideWindow.h"
#include <FreeMem.h>
#include <SPI.h>

//UTouch myTouch(6,5,32,3,2);

/*********************************************
*********************************************/


/*********************************************
*********************************************/
CWindows::CWindows(UTFT &utft, UTouch &utouch):
	tft(utft), 
	touch(utouch), 
	touchinput(utft, utouch), 
	scenario(utft),
	splashWindow(/*getSplashArea(), */utft, scenario, NULL),
	sideWindow(/*getSideWindowArea(), */utft, scenario, NULL),
	mainWindow(/*getMainWindowArea(), */utft, scenario, (uint8_t*)g_katiline_bg)
{
	//touchinput = new TouchInput(tft, touch);
	//scenario = new Scenario(tft);

	//splashWindow = new Splash(getSplashArea(), getUTFT(), getScenario(), NULL);
	//sideWindow = new SideWindow(getSideWindowArea(), getUTFT(), getScenario(), NULL);
	//mainWindow = new WindowKatiline(getMainWindowArea(), getUTFT(), getScenario(), (uint8_t*)g_katiline_bg);

	currWindow = CurrWindowNone;
	Reset();
}

CWindows::~CWindows()
{
}

void CWindows::Reset()
{
	//Events::Reset();
	splashWindow.Reset();
	sideWindow.Reset();
	mainWindow.Reset();
}

void CWindows::InitHW()
{
	//Log.debug("CWindows::InitHW()");
	/*
	uint8_t dmodel = CPLD;
	uint8_t ficid = ReadFontIC_ID();
	Log.debug("FontIC ID: " + string(ficid));
	switch (ficid)
	{
	case 0:
		dmodel = CPLD;
		break;
	case 1:
	case 2:
		dmodel = CTE70;
		break;
	default:
		break;
	}
	*/
	////Initialize TFT 
	pinMode(52, OUTPUT);
	digitalWrite(52, HIGH);

	tft.InitLCD();

	tft.setColor(0, 0, 0);
	tft.setBackColor(255, 255, 255);
	tft.fillScr(255, 255, 255);

	availableArea.Left = 0;
	availableArea.Top = 0;
	availableArea.Right = tft.getDisplayXSize() - 1;
	availableArea.Bottom = tft.getDisplayYSize() - 1;

	side_area.Left = availableArea.Right - SIDE_WINDOW_WIDTH;
	side_area.Top = availableArea.Top;
	side_area.Right = availableArea.Right;
	side_area.Bottom = availableArea.Bottom;

	main_area.Left = availableArea.Left;
	main_area.Top = availableArea.Top;
	main_area.Right = availableArea.Right - SIDE_WINDOW_WIDTH;
	main_area.Bottom = availableArea.Bottom;
}

void CWindows::Begin()
{
	TimeSlice.RegisterTimeSlice(this);
	last_reinit = millis();
}

void CWindows::BeginSplashWindow(void)
{
	splashWindow.Begin(GetSplashArea());
}

void CWindows::BeginSideWindow(void)
{
	sideWindow.Begin(GetSideWindowArea());
}

void CWindows::BeginMainWindow(void)
{
	mainWindow.Begin(GetMainWindowArea());
}

void CWindows::ReInitTFT(){
	tft.InitLCD();
	switch (currWindow)
	{
	case CurrWindowNone:
		ClearScreen();
		break;
	case CurrWindowsSplash:
		splashWindow.Draw();
		break;
	case CurrWindowMain:
		mainWindow.Draw();
		break;
	default:
		break;
	}
	last_reinit = millis();
}

unsigned char CWindows::ReadFontIC_ID()
{
	unsigned char ID_0,ID_1;			
	digitalWrite(52,LOW);
	SPI.transfer(0x90);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);

	delayMicroseconds(10);
	ID_0=SPI.transfer(0xFF);
    ID_1=SPI.transfer(0xFF);
    digitalWrite(52,HIGH);
    if (ID_1==0x15) return 1;
    else if (ID_1==0x16) return 2;
    else return 0;
}

UTFT& CWindows::getUTFT()
{
	return tft;
}

Scenario& CWindows::getScenario()
{
	return scenario;
}

Rect& CWindows::GetSplashArea()
{
	return availableArea;
}

Rect& CWindows::GetSideWindowArea()
{
	return side_area;
}

Rect& CWindows::GetMainWindowArea()
{
	return main_area;
}

void CWindows::ClearScreen()
{
	tft.fillScr(255, 255, 255);	
}

Splash & CWindows::GetSplashScreen()
{
	return splashWindow;
}

WindowKatiline & CWindows::GetMainWindow()
{
	return mainWindow;
}

SideWindow & CWindows::GetSideWindow()
{
	return sideWindow;
}

void CWindows::DrawSplashScreen()
{
	currWindow = CurrWindowsSplash;
	splashWindow.Draw();
}

void CWindows::DrawMainWindow()
{
	currWindow = CurrWindowMain;
	mainWindow.Draw();
}

void CWindows::DrawSideWindow()
{
	sideWindow.Draw();
}

void CWindows::OnTimeSlice()// TimeEvent(uint32_t t)
{
	uint32_t t = millis();
	if (currWindow == CurrWindowMain)
	{
		sideWindow.TimeEvent(t);
		mainWindow.TimeEvent(t);		
	}

	if(touchinput.MsgAvailable())
	{	  
		switch (currWindow)
		{
		case CurrWindowNone:
			break;
		case CurrWindowsSplash:
			splashWindow.Message(touchinput.GetMessage());
			break;
		case CurrWindowMain:
			mainWindow.Message(touchinput.GetMessage());
			sideWindow.Message(touchinput.GetMessage());
			break;
		default:
			break;
		}
	}
	//

	switch (tft.display_model)
	{
	case CTE70:
		if(abs(t-last_reinit) > 1000*60*5){
			ReInitTFT();
		}
		break;
	default:
		break;
	}
}

void CWindows::ForceLCDBacklight()
{
	touchinput.ForceLCDBacklight();
}

