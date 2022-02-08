#pragma once

#include "UTFT.h"
#include "Events.h"
#include "win_defs.h"
#include "msg.h"
#include "UI.h"
#include "Scenario.h"

/*********************************************
*********************************************/

class CWindow: public Events
{
public:
CWindow(Rect windowarea, UTFT& utft, Scenario& Sc, byte* scbuf);
~CWindow(void);
String Name;
bool IsActive();
void Acivate();
void Deactivate();
void Message(Msg msg);
virtual void TimeEvent(uint32_t t);
Rect getWindowArea();
Rect getClientArea();
void SetColor(RGBColor clr);
void SetBorderColor(RGBColor clr);
void Draw();
void DrawArea(Rect area);
void AddUIElement(UIBase* element);
UIBase* GetUIElement(UIID* id);
void PlayScenario(int x, int y, byte* buf);
Scenario& getScenario();
void DisableWindow();
void EnableWindow();
UTFT& GetUTFT();
//UTFT_CTE* GetUTFT_CTE();
ModalResult IsSetModalResult();
virtual char* GetModalResult();
void Invalidate();
protected:
UTFT& tft;
List<UIBase> uielements;
Scenario& scenario;
byte* sc_buf;
Rect windowArea;
Rect clientArea;
RGBColor clr;
RGBColor bclr;
ModalResult mresult;
bool window_drawn;
bool UIInRect(UIBase* elem, Rect r);
bool PointInRect(int x, int y, Rect r);
void setClientArea();
};

