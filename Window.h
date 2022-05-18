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
private:
	bool active;
public:
CWindow(/*Rect windowarea, */UTFT& utft, Scenario& Sc, uint8_t* scbuf);
virtual ~CWindow(void);
char Name[MAX_NAME_LENGTH];

void Message(Msg msg);
virtual void TimeEvent(uint32_t t);
bool IsActive(void);
void SetWindowArea(Rect& r);
Rect getWindowArea();
Rect getClientArea();
void SetColor(RGBColor clr);
void SetBorderColor(RGBColor clr);
void Draw();
void AddUIElement(UIBase* element);
UIBase* GetUIElement(UIID* id);
void PlayScenario(int x, int y, uint8_t* buf);
Scenario& getScenario();
void DisableWindow();
void EnableWindow();
UTFT& GetUTFT();
void Invalidate();
virtual void Begin();
void Reset(void);
protected:
UTFT& tft;
List<UIBase> uielements;
Scenario& scenario;
uint8_t* sc_buf;
Rect windowArea;
Rect clientArea;
RGBColor clr;
RGBColor bclr;
bool window_drawn;
void setClientArea();
};

