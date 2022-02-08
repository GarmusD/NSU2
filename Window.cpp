#include "Window.h"

/*********************************************
*********************************************/
CWindow::CWindow(Rect windowarea, UTFT& utft, Scenario& Sc, byte* scbuf) :tft(utft), scenario(Sc)
{
	//tft = utft;
	//tft = cte->_UTFT;
	//_scenario = Sc;
	sc_buf = scbuf;
	windowArea = windowarea;
	setClientArea();
	clr = WindowBackgroudColor;
	bclr = WindowBorderColor;
	mresult = MR_NO_RESULT;
	window_drawn = false;
}

CWindow::~CWindow(void)
{
	while (uielements.Count() > 0)
	{
		Serial.println("DBG: ~Window. UIElements count: " + String(uielements.Count()));
		UIBase* uielem = uielements.Get(0);
		delete uielem;
		//Serial.println("DBG: ~Window. UIElement deleted");
		uielements.Delete(0);
		//Serial.println("DBG: ~Window. Deleted from list.");
	}
	//Serial.println("DBG: ~Window. Done");
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
		if (String((char*)uielem->getUIID()).compareTo(String((char*)id)))
		{
			return uielem;
		}
	}
	return NULL;
}

void CWindow::Draw()
{
	Log.debug("CWindow::Draw()");
	tft.setColor(clr.R, clr.G, clr.B);
	Log.debug("2");
	tft.fillRoundRect(windowArea.Left, windowArea.Top, windowArea.Right, windowArea.Bottom);
	Log.debug("3");
	tft.setColor(bclr.R, bclr.G, bclr.B);
	Log.debug("4");
	tft.drawRoundRect(windowArea.Left, windowArea.Top, windowArea.Right, windowArea.Bottom);
	Log.debug("5");
	tft.drawRoundRect(windowArea.Left + 2, windowArea.Top + 2, windowArea.Right - 2, windowArea.Bottom - 2);
	Log.debug("Uchhh :)");
	//draw scenario
	Log.debug("CWindow::Draw() PlayScenario()");
	if (sc_buf != NULL) scenario.PlayScenario(clientArea.Left, clientArea.Top, sc_buf);
	//draw Graphics components
	//draw UI components
	Log.debug("UIElements count: "+String(uielements.Count()));
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
	Log.debug("Draw DONE :)");
}

void CWindow::Invalidate()
{
	if (window_drawn) {
		Draw();
	}
}

void CWindow::DisableWindow()
{
	for (int i = 0; i < uielements.Count(); i++)
	{
		UIBase* uielem = uielements.Get(i);
		uielem->DisableDrawing(true);
	}
}

void CWindow::EnableWindow()
{
	for (int i = 0; i < uielements.Count(); i++)
	{
		UIBase* uielem = uielements.Get(i);
		uielem->DisableDrawing(false);
	}
}

void CWindow::DrawArea(Rect area)
{
	tft.setColor(clr.R, clr.G, clr.B);
	tft.fillRect(area.Left, area.Top, area.Right, area.Bottom);
	tft.setColor(bclr.R, bclr.G, bclr.B);
	//draw scenario
	if (sc_buf != NULL) scenario.PlayScenario(clientArea.Left, clientArea.Top, area, sc_buf);
	//draw Graphics components
	//draw UI components
	for (int i = 0; i < uielements.Count(); i++)
	{
		UIBase* uielem = uielements.Get(i);
		if (UIInRect(uielem, area))
		{
			uielem->Invalidate();
		}
	}
}

bool CWindow::UIInRect(UIBase* elem, Rect r)
{
	int ex, ey, ew, eh;
	elem->GetPosition(ex, ey);
	elem->GetSize(ew, eh);

	int left1, left2;
	int right1, right2;
	int top1, top2;
	int bottom1, bottom2;

	ex += windowArea.Left;
	ey += windowArea.Top;

	left1 = ex;
	left2 = r.Left;

	right1 = ex + ew;
	right2 = r.Right;

	top1 = ey;
	top2 = r.Top;

	bottom1 = ey + eh;
	bottom2 = r.Bottom;

	return !(
		(bottom1 < top2) | (top1 > bottom2) | (right1 < left2) | (left1 > right2)
		);
}

bool CWindow::PointInRect(int x, int y, Rect r)
{
	return x >= r.Left && x <= r.Right && y >= r.Top && y <= r.Bottom;
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

void CWindow::PlayScenario(int x, int y, byte* buf)
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

/*
UTFT_CTE* Window::GetUTFT_CTE()
{
return cte;
}
*/

ModalResult CWindow::IsSetModalResult()
{
	return mresult;
}

char* CWindow::GetModalResult()
{
	return 0;
}