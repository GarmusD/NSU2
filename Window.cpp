#include "Window.h"

/*********************************************
*********************************************/
CWindow::CWindow(/*Rect windowarea, */UTFT& utft, Scenario& Sc, uint8_t* scbuf) :tft(utft), scenario(Sc)
{
	//tft = utft;
	//tft = cte->_UTFT;
	//_scenario = Sc;
	sc_buf = scbuf;
	windowArea.Bottom = 0;
	windowArea.Left = 0;
	windowArea.Right = 0;
	windowArea.Top = 0;

	clientArea.Bottom = 0;
	clientArea.Left = 0;
	clientArea.Right = 0;
	clientArea.Top = 0;
	
	clr = WindowBackgroudColor;
	bclr = WindowBorderColor;
	window_drawn = false;
}

CWindow::~CWindow(void)
{
	char s[64];
	while (uielements.Count() > 0)
	{
		sprintf(s, "0 DBG: ~Window(). UIElements count: %d", uielements.Count());
		Serial.println(s);
		UIBase* uielem = uielements.Get(0);
		delete uielem;
		uielements.Delete((uint8_t)0);
	}
	uielements.Clear();
}

void CWindow::Begin()
{
}

void CWindow::Reset(void)
{
	active = false;
	window_drawn = false;
}

void CWindow::Message(Msg msg)
{
	for (int i = 0; i < uielements.Count(); i++)
	{
		UIBase* uielem = uielements.Get(i);
		uielem->Message(msg);
	}
}

void CWindow::TimeEvent(uint32_t t)
{
	for (int i = 0; i < uielements.Count(); i++)
	{
		UIBase* uielem = uielements.Get(i);
		uielem->TimeEvent(t);
	}
}
bool CWindow::IsActive(void)
{
	return active;
}
void CWindow::SetWindowArea(Rect & r)
{
	windowArea.Top = r.Top;
	windowArea.Left = r.Left;
	windowArea.Bottom = r.Bottom;
	windowArea.Right = r.Right;
	setClientArea();
}
//TODO - prideti elementus pagal eiliskuma, piesti pagal eiliskuma, msg sukti atbuline tvarka
void CWindow::AddUIElement(UIBase* element)
{
	uielements.Add(element);
}

UIBase* CWindow::GetUIElement(UIID* id)
{
	for (int i = 0; i < uielements.Count(); i++)
	{
		UIBase* uielem = uielements.Get(i);
		if (strcmp((char*)uielem->getUIID(), (char*)id) == 0)
		{
			return uielem;
		}
	}
	return NULL;
}

void CWindow::Draw()
{
	active = true;
	tft.setColor(clr.R, clr.G, clr.B);
	tft.fillRoundRect(windowArea.Left, windowArea.Top, windowArea.Right, windowArea.Bottom);
	tft.setColor(bclr.R, bclr.G, bclr.B);
	tft.drawRoundRect(windowArea.Left, windowArea.Top, windowArea.Right, windowArea.Bottom);
	tft.drawRoundRect(windowArea.Left + 2, windowArea.Top + 2, windowArea.Right - 2, windowArea.Bottom - 2);
	//draw scenario
	if (sc_buf != NULL) scenario.PlayScenario(clientArea.Left, clientArea.Top, sc_buf);
	//draw Graphics components
	char s[64];
	sprintf(s, "CWindow::Draw() - UIElements: %d", uielements.Count());
	Log.debug(s);
	//draw UI components
	for (int i = 0; i < uielements.Count(); i++)
	{
		UIBase* uielem = uielements.Get(i);
		if (uielem) {
			//Serial.println("DBG: UIElements not NULL.");
			uielem->Draw();//>Invalidate();
		}
		else {
			//Serial.println("DBG: UIElements IS NULL!!!");
		}
	}
	window_drawn = true;
}

void CWindow::Invalidate()
{
	if (window_drawn) {
		Draw();
	}
}

void CWindow::DisableWindow()
{
	active = false;
	//for (int i = 0; i < uielements.Count(); i++)
	//{
	//	UIBase* uielem = uielements.Get(i);
	//	uielem->DisableDrawing(true);
	//}
}

void CWindow::EnableWindow()
{
	active = true;
	//for (int i = 0; i < uielements.Count(); i++)
	//{
	//	UIBase* uielem = uielements.Get(i);
	//	uielem->DisableDrawing(false);
	//}
}

void CWindow::setClientArea()
{
	clientArea.Left = windowArea.Left + 5;
	clientArea.Top = windowArea.Top + 5;
	clientArea.Right = windowArea.Right - 5;
	clientArea.Bottom = windowArea.Bottom - 5;
}

Rect CWindow::getWindowArea()
{
	return windowArea;
}

Rect CWindow::getClientArea()
{
	return clientArea;
}

void CWindow::SetColor(RGBColor clr)
{
	this->clr = clr;
}

void CWindow::SetBorderColor(RGBColor clr)
{
	this->bclr = clr;
}

void CWindow::PlayScenario(int x, int y, uint8_t* buf)
{
	scenario.PlayScenario(x, y, buf);
}

Scenario& CWindow::getScenario()
{
	return scenario;
}

UTFT& CWindow::GetUTFT()
{
	return tft;
}