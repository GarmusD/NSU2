#ifndef WindowSplash_h
#define WindowSplash_h

#include "windows.h"
#include "UI.h"

class Splash: public CWindow
{
public:
	Splash(Rect windowarea, UTFT& utft, Scenario& Sc, byte* sc_buf);
	~Splash(void);
	void SetInfoText(String tekstas);
	void UpdateInfoText(String tekstas);
private:
	//UI
	UILabel infolbl, infolbl2, infolbl3;
};

#endif
