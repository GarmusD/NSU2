#include "windows.h"
#include "win_defs.h"
#include "UI.h"
#include <UTouch.h>
#include "InputWindow.h"
#include "SideWindow.h"
#include <FreeMem.h>
#include <SPI.h>

//UTouch myTouch(6,5,32,3,2);

/*********************************************
*********************************************/


/*********************************************
*********************************************/
CWindowInfo::CWindowInfo(CWindow* win, String name)
{
	Window = win;
	Name = name;
	Active = false;
}

/*********************************************
*********************************************/
CWindows::CWindows(UTFT &utft, UTouch &utouch):tft(utft), touch(utouch)/*:tft(CPLD, 25, 26, 27, 28), touchinput(tft), scenario(tft)*/
{
	
}

void CWindows::Begin()
{
	TimeSlice.RegisterTimeSlice(this);

	byte dmodel = CPLD;
	byte ficid = ReadFontIC_ID();
	Log.debug("FontIC ID: " + String(ficid));
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
	//tft = new UTFT(dmodel, 25, 26, 27, 28);   // Remember to change the model parameter to suit your display module!
	Log.debug("WindowsClass::Begin() -> tft addr: "+String((uint32_t)(void *)&tft));
	////fonts = Fonts::getInstance(tft);
	////cte = new UTFT_CTE(tft);
	////Initialize TFT 
	tft.InitLCD();
	////cte->SPI_Flash_init(FLASH_CS_PIN);//, SPI_RATE);
	////cte->show_color_bar();
	tft.fillScr(255, 255, 255);
	////tft->Swtich_LCD_TB_Bit(1); 
	////fonts->Set_character_spacing(1);
	tft.setColor(0, 0, 0);  
	tft.setBackColor(255, 255, 255);
	
	touchinput = new TouchInput(tft, touch);

	scenario = new Scenario(tft);

	sw_area_calculated = false;
	sw_enabled = false;//side window

	availableArea.Left = 0;
	availableArea.Top = 0;
	availableArea.Right = tft.getDisplayXSize()-1;
	availableArea.Bottom = tft.getDisplayYSize()-1;
	currentWindow = NULL;
	previousWindow = NULL;
	modalWindow = NULL;
	sideWindow = NULL;
	last_reinit = millis();
}

void CWindows::ReInitTFT(){
	tft.InitLCD();
	if(currentWindow){
		currentWindow->Draw();
	}
	if(sideWindow){
		sideWindow->Draw();
	}
	last_reinit = millis();
}

CWindows::~CWindows(void)
{
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

/*
UTFT_CTE* Windows::getUTFT_CTE()
{
	return cte;
}
*/

Scenario& CWindows::getScenario()
{
	return *scenario;
}

Rect CWindows::getAvailableArea()
{
	return availableArea;
}

Rect CWindows::getSideWindowArea()
{
	if(!sw_area_calculated)
	{
		sw_area.Left = availableArea.Right-SIDE_WINDOW_WIDTH;
		sw_area.Top = availableArea.Top;
		sw_area.Right = availableArea.Right;
		sw_area.Bottom = availableArea.Bottom;
	
		availableArea.Right -= SIDE_WINDOW_WIDTH;
	
		sw_area_calculated = true;
	}
	return sw_area;
}

void CWindows::ClearScreen()
{
	Log.debug("ClearScreen()");
	if(sideWindow != NULL)
	{
		Log.debug("if(sideWindow != NULL)");
		Log.debug("tft.setColor(255, 255, 255);");
		tft.setColor(255, 255, 255);
		Log.debug("tft.fillRect(availableArea.Left, availableArea.Top, availableArea.Right, availableArea.Bottom);");
		tft.fillRect(availableArea.Left, availableArea.Top, availableArea.Right, availableArea.Bottom);
	}
	else
	{
		Log.debug("tft.fillScr(255, 255, 255);");
		
		//Log.debug("WindowsClass::ClearScreen() -> tft addr: " + String((uint32_t)(void *)&tft));
		tft.clrScr();
		//tft.fillScr(255, 255, 255);
		//tft.fillScr(0xFFFF);
		
		Log.debug("tft->fillScr(255, 255, 255); DONE.");
	}
	Log.debug("Done ClearScreen()");
}

void CWindows::DrawCurrentWindow()
{
	if(currentWindow != NULL) currentWindow->Draw();
}

//void Windows::CreateSideWindow()
//{
//	Rect r;
//	r.Left = availableArea.Right-SIDE_WINDOW_WIDTH;
//	r.Top = availableArea.Top;
//	r.Right = availableArea.Right;
//	r.Bottom = availableArea.Bottom;
//	sideWindow = new SideWindow(r, cte, scenario, NULL);//<- change to SideWindow scenario
//	
//	availableArea.Right -= SIDE_WINDOW_WIDTH;
//	sideWindow->SetColor(SideWindowBackgroudColor);
//	sideWindow->SetBorderColor(SideWindowBorderColor);
//	sideWindow->Draw();
//	sw_enabled = true;
//}

CWindow* CWindows::CreateWindow()
{
	if(currentWindow != NULL)
	{
		if(previousWindow != NULL) CloseWindow(previousWindow);
		previousWindow = currentWindow;
	}
	currentWindow = new CWindow(availableArea, tft, *scenario, NULL);
	return currentWindow;
}

CWindow* CWindows::AddWindow(CWindow* createdWindow)
{
	//Serial.print("DBG: Free mem in windows->AddWindow(): "); Serial.println(FreeMem::GetFreeMem());
	
	if(currentWindow != NULL)
	{
		if(previousWindow != NULL) CloseWindow(previousWindow);
		previousWindow = currentWindow;
	}
	currentWindow = createdWindow;
	return currentWindow;
}

void CWindows::AddSideWindow(CWindow* swindow)
{
	if(sideWindow){
		delete sideWindow;
	}
	sideWindow = swindow;
	sideWindow->SetColor(SideWindowBackgroudColor);
	sideWindow->SetBorderColor(SideWindowBorderColor);
	
	sw_enabled = true;
	sideWindow->Draw();
}

CWindow* CWindows::CreateWindow(byte* sc_buf)
{
	if(currentWindow)
	{
		if(previousWindow) CloseWindow(previousWindow);
		previousWindow = currentWindow;
	}
	currentWindow = new CWindow(availableArea, tft, *scenario, sc_buf);
	return currentWindow;
}

CWindow* CWindows::CreateModal()
{
	sw_enabled = false;
	InputWindow* iw = new InputWindow(availableArea, tft, *scenario, NULL);
	if(currentWindow) currentWindow->DisableWindow();
	iw->Draw();
	iw->SetInputFocus();
	modalWindow = iw;
	return modalWindow;
}

void CWindows::CloseWindow(CWindow* win)
{
	Log.debug("void CWindows::CloseWindow(Window* win)");
	if(currentWindow == win)
	{
		if(previousWindow)
		{
			Log.debug("previousWindow->Draw();");
			previousWindow->Draw();//Area(win->getWindowArea());
			currentWindow = previousWindow;
			previousWindow = NULL;
		}
		else
		{
			Log.debug("previousWindow = NULL; ClearScreen(); ");
			currentWindow = NULL;
			ClearScreen();
		}			
	}
	Log.debug("delete win)");
	delete win;
	Log.debug("void WindowsClass::CloseWindow(Window* win) DONE");
}

void CWindows::OnTimeSlice()// TimeEvent(uint32_t t)
{
	//Log.debug("WindowsClass::OnTimeSlice()");
	uint32_t t = millis();

	if(sw_enabled) sideWindow->TimeEvent(t);
	if(modalWindow != NULL)
	{
		modalWindow->TimeEvent(t);
	}
	else if(currentWindow != NULL) currentWindow->TimeEvent(t);

	if(touchinput->MsgAvailable())
	{	  
		if(sw_enabled) sideWindow->Message(touchinput->GetMessage());
		if(modalWindow)
		{
			modalWindow->Message(touchinput->GetMessage());
		}
		else
		{
			if(currentWindow) currentWindow->Message(touchinput->GetMessage());
		}
	}
	//
	if(modalWindow)
	{
		if(modalWindow->IsSetModalResult() != MR_NO_RESULT)
		{
			//get result
			//redraw
			if(currentWindow)
			{
				currentWindow->DrawArea(modalWindow->getWindowArea());
				currentWindow->EnableWindow();
			}
			if(sideWindow)
			{
				sw_enabled = true;
			}
			delete modalWindow;
			modalWindow = NULL;
		}
	}
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

void CWindows::DisableSideWindow()
{
	sw_enabled = false;
}

void CWindows::EnableSideWindow()
{
	sw_enabled = true;
}

void CWindows::ForceLCDBacklight()
{
	touchinput->ForceLCDBacklight();
}

/*********************************************
*********************************************/

//CWindows Windows;
