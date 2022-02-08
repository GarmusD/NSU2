#include "SideWindow.h"


SideWindow::SideWindow(Rect windowarea, UTFT& utft, Scenario& Sc, byte* sc_buf):CWindow(windowarea, utft, Sc, sc_buf)
{
	Log.debug("Creating side window.");
	timewidget = new TimeWidget(this);
	Log.debug("TimeWidgetCreated...");
	timewidget->SetSize(this->clientArea.Right-this->clientArea.Left-4, 26);
	timewidget->SetColor(COLOR_WHITE, SideWindowBackgroudColor);
	timewidget->textAlign = AlignCenter;

	int tww = 0, twh = 0;
	timewidget->GetSize(tww, twh);
	Rect area = this->getClientArea();
	timewidget->SetPosition(2,  area.Bottom - twh - 2);
	timewidget->SetUIID("laikas");
	Log.debug("this->AddUIElement(timewidget);...");
	this->AddUIElement(timewidget);
	Log.debug("this->AddUIElement(timewidget); done");
}


SideWindow::~SideWindow(void)
{
	delete timewidget;
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