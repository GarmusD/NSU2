#ifndef WindowKatiline_h
#define WindowKatiline_h

#include "windows.h"
#include "Events.h"
#include "UI.h"
#include "MAX31855.h"
#include "WoodBoiler.h"

class WindowKatiline :	public CWindow
{
public:
	WindowKatiline(Rect windowarea, UTFT& utft, Scenario& Sc, byte* sc_buf);
	~WindowKatiline(void);
	void TimeEvent(uint32_t t);
private:
	int g_count, btn_count;
	Status kstatus;
	String GetKatilasStatusText(Status value);
	bool vent_on, vent_manual, ladom_on;
	//UI
	UILabel* lbl;
	UIGraphics *g, *katilas, *akum, *kaminas, *kol1, *kol2, *bg;
	UIGraphics *trisakis, *radiatorius, *grindys, *boileris;
	UIVentilator *ventiliatorius;
	UILadomat *ladom;
	UIButton *btn;
	UITempBar *tempbar;
	UICircPump *cirk1, *cirk2, *cirk3;
	UITempLabel *ktemp, *ktemp2, *akumt1, *akumt2, *akumt3, *boilert_virsus, *boilert_apacia, *chimneytemp;

	void HandleOnClick(void* Sender, int x, int y, int tag);
	//MAX31855* ktype;
	void HandleStatusChange(void* Sender, Status status);
};

#endif
