#include "WindowSplash.h"


Splash::Splash(/*Rect windowarea, */UTFT& utft, Scenario& Sc, uint8_t* sc_buf) :CWindow(/*windowarea, */utft, Sc, sc_buf), infolbl(this), infolbl2(this), infolbl3(this)
{
	Log.debug("Creating Splash window...");

	
	Log.debug("Splash window created...");
}


Splash::~Splash(void)
{
}

void Splash::Begin(Rect& area)
{
	SetWindowArea(area);

	Rect ca = getClientArea();
	infolbl.SetPosition(ca.Left + 2, ca.Bottom - 55);
	infolbl.SetSize(ca.Right - 4, 26);
	infolbl.SetUIID(const_cast<char *>("infolbl3"));
	infolbl.textAlign = AlignCenter;

	infolbl2.SetPosition(ca.Left + 2, ca.Bottom - 85);
	infolbl2.SetSize(ca.Right - 4, 26);
	infolbl2.SetUIID(const_cast<char *>("infolbl2"));
	infolbl2.textAlign = AlignCenter;

	infolbl3.SetPosition(ca.Left + 2, ca.Bottom - 115);
	infolbl3.SetSize(ca.Right - 4, 26);
	infolbl3.SetUIID(const_cast<char *>("infolbl1"));
	infolbl3.textAlign = AlignCenter;
}

void Splash::Reset()
{
	infolbl3.SetCaption("");
	infolbl2.SetCaption("");
	infolbl.SetCaption("");
}

void Splash::SetInfoText(const char* tekstas)
{
	infolbl3.SetCaption(infolbl2.GetCaption());
	infolbl2.SetCaption(infolbl.GetCaption());
	infolbl.SetCaption(tekstas);
}

void Splash::UpdateInfoText(const char* tekstas)
{
	infolbl.SetCaption(tekstas);	
}