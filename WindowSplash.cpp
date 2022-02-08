#include "WindowSplash.h"


Splash::Splash(Rect windowarea, UTFT& utft, Scenario& Sc, byte* sc_buf) :CWindow(windowarea, utft, Sc, sc_buf), infolbl(this), infolbl2(this), infolbl3(this)
{
	Log.debug("Creating Splash window...");

	//infolbl = new UILabel(this);
	//infolbl2 = new UILabel(this);
	//infolbl3 = new UILabel(this);

	Rect ca = getClientArea();
	infolbl.SetPosition(ca.Left + 2, ca.Bottom - 55);
	infolbl.SetSize(ca.Right-4, 26);
	infolbl.SetUIID(const_cast<char *>("infolbl"));
	infolbl.textAlign = AlignCenter;

	infolbl2.SetPosition(ca.Left + 2, ca.Bottom - 85);
	infolbl2.SetSize(ca.Right-4, 26);
	infolbl2.SetUIID(const_cast<char *>("infolbl"));
	infolbl2.textAlign = AlignCenter;

	infolbl3.SetPosition(ca.Left + 2, ca.Bottom - 115);
	infolbl3.SetSize(ca.Right-4, 26);
	infolbl3.SetUIID(const_cast<char *>("infolbl"));
	infolbl3.textAlign = AlignCenter;
	Log.debug("Splash window created...");
}


Splash::~Splash(void)
{
	//delete infolbl3;
	//delete infolbl2;
	//delete infolbl;
}

void Splash::SetInfoText(String tekstas)
{
	Log.debug("lbl2 -> lbl3");
	infolbl3.SetCaption(infolbl2.GetCaption());
	Log.debug("lbl1 -> lbl2");
	infolbl2.SetCaption(infolbl.GetCaption());
	Log.debug("lbl1 = tekstas");
	infolbl.SetCaption(tekstas);
	Log.debug("SetInfoText DONE");
}

void Splash::UpdateInfoText(String tekstas)
{
	infolbl.SetCaption(tekstas);	
}