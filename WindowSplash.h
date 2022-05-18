#ifndef WindowSplash_h
#define WindowSplash_h

#include "window.h"
#include "UI.h"

class Splash: public CWindow
{
public:
	Splash(/*Rect windowarea, */UTFT& utft, Scenario& Sc, uint8_t* sc_buf);
	~Splash(void);
	void SetInfoText(const char* tekstas);
	void UpdateInfoText(const char* tekstas);
	void Begin(Rect& area);
	void Reset();
private:
	//UI
	UILabel infolbl, infolbl2, infolbl3;
};

#endif
