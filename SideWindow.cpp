#include "SideWindow.h"


SideWindow::SideWindow(/*Rect windowarea, */UTFT& utft, Scenario& Sc, uint8_t* sc_buf):CWindow(/*windowarea, */utft, Sc, sc_buf),
	timewidget(this), btnOpMode(this), btnTempMode(this), btnKarstasVanduo(this)
{
	Log.debug("Creating side window.");
	//timewidget = new TimeWidget(this);
	//Log.debug("TimeWidgetCreated...");
	

	//UISwitchButton* btnOpMode = new UISwitchButton(this);
	btnOpMode.SetOnBMPBytes(snaige_bmp);
	btnOpMode.SetOffBMPBytes(saule_bmp);
	

	//Log.debug("Creating UISwitchButton* btnTempMode...");
	//UISwitchButton* btnTempMode = new UISwitchButton(this);
	btnTempMode.SetOnBMPBytes(temp_up_bmp);
	btnTempMode.SetOffBMPBytes(temp_down_bmp);
	

	//Log.debug("Creating UISwitchButton* btnKarstasVanduo...");
	//UISwitchButton* btnKarstasVanduo = new UISwitchButton(this);
	btnKarstasVanduo.SetOnBMPBytes(karstas_vanduo_on_bmp);
	btnKarstasVanduo.SetOffBMPBytes(karstas_vanduo_off_bmp);
	
	AddButton(&btnOpMode);
	AddButton(&btnTempMode);
	AddButton(&btnKarstasVanduo);
}


SideWindow::~SideWindow(void)
{
	//delete timewidget;
}

void SideWindow::Begin(Rect& area)
{
	Log.debug("SideWindow::Begin()");
	SetWindowArea(area);

	timewidget.SetSize(this->clientArea.Right - this->clientArea.Left - 4, 26);
	timewidget.SetColor(COLOR_WHITE, SideWindowBackgroudColor);
	timewidget.textAlign = AlignCenter;

	int tww = 0, twh = 0;
	timewidget.GetSize(tww, twh);
	timewidget.SetPosition(2, this->clientArea.Bottom - twh - 2);
	timewidget.SetUIID("laikas");
	//Log.debug("this->AddUIElement(timewidget);...");
	//this->AddUIElement(timewidget);
	//Log.debug("this->AddUIElement(timewidget); done");

	btnOpMode.SetSwitch(Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_WINTER_MODE]));
	btnTempMode.SetSwitch(Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_TEMPERATURE_MODE]));
	btnKarstasVanduo.SetSwitch(Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_HOT_WATER_MODE]));
}

void SideWindow::AddButton(UIImgButton* btn)
{
	int new_y = 1;
	int w, h;
	for(int i = 0; i < buttons.Count(); i++)
	{
		UIImgButton* b = buttons.Get(i);
		b->GetSize(w, h);
		new_y += h + BTN_SPACE;
	}
	btn->SetPosition(1, new_y);
	buttons.Add(btn);
	btn->Invalidate();
}

//void SideWindow::TimeEvent(uint32_t t)
//{
//}