#ifndef WindowKatiline_h
#define WindowKatiline_h

#include "Window.h"
#include "Events.h"
#include "UI.h"
#include "MAX31855.h"
#include "WoodBoilers.h"

class WindowKatiline :	public CWindow
{
public:
	WindowKatiline(/*Rect windowarea, */UTFT& utft, Scenario& Sc, uint8_t* sc_buf);
	~WindowKatiline(void);
	void Begin(Rect& area);//Attach handlers
	void TimeEvent(uint32_t t);
private:
	int g_count, btn_count;
	Status kstatus;
	bool vent_on, vent_manual, ladom_on;
	//
	MAX31855* ktype;
	CWoodBoiler* woodb;
	//UI
	UILabel lbl;
	UIGraphics katilas, akum, kaminas, kol1, kol2;
	UIGraphics trisakis, radiatorius, grindys, boileris;
	UIVentilator ventiliatorius;
	UILadomat ladom;
	UIButton btn;
	UITempBar tempbar;
	UICircPump cirk1, cirk2, cirk3;
	UITempLabel ktemp, ktemp2, akumt1, akumt2, akumt3, boilert_virsus, boilert_apacia, chimneytemp;

	/*
	UILabel* lbl;
	UIGraphics *g, *katilas, *akum, *kaminas, *kol1, *kol2, *bg;
	UIGraphics *trisakis, *radiatorius, *grindys, *boileris;
	UIVentilator *ventiliatorius;
	UILadomat *ladom;
	UIButton *btn;
	UITempBar *tempbar;
	UICircPump *cirk1, *cirk2, *cirk3;
	UITempLabel *ktemp, *ktemp2, *akumt1, *akumt2, *akumt3, *boilert_virsus, *boilert_apacia, *chimneytemp;
	*/

	void HandleOnClick(void* Sender, int x, int y, int tag);
	//MAX31855* ktype;
	void HandleStatusChange(void* Sender, Status status);
};

#endif
