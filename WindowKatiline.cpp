#include "WindowKatiline.h"
#include "graphics.h"
//#include "bmp_saule.h"
#include "TSensors.h"

WindowKatiline::WindowKatiline(/*Rect windowarea, */UTFT& utft, Scenario& Sc, uint8_t* sc_buf):CWindow(/*windowarea, */utft, Sc, sc_buf),
	lbl(this), katilas(this), akum(this), kaminas(this), kol1(this), kol2(this),
	trisakis(this), radiatorius(this), grindys(this), boileris(this),
	ventiliatorius(this), ladom(this), btn(this), tempbar(this),
	cirk1(this), cirk2(this), cirk3(this),
	ktemp(this), ktemp2(this), akumt1(this), akumt2(this), akumt3(this), boilert_virsus(this), boilert_apacia(this), chimneytemp(this)
{
	//Log.debug("Katiline Constructor done.");
}


WindowKatiline::~WindowKatiline(void)
{
	//WoodBoilers.Get(0)->RemoveStatusChangeHandler(this);
	//All UI is deleted in base class
}

void WindowKatiline::Begin(Rect& area)
{
	Log.debug("WindowKatiline::Begin()");
	SetWindowArea(area);

	kaminas.SetSize(41, 122);
	kaminas.SetPosition(41, 12);
	kaminas.SetVectorBytes((uint8_t*)g_kaminas);
	kaminas.SetUIID("Kaminas");

	ventiliatorius.SetPosition(45, 145);
	ventiliatorius.SetUIID("vent");

	katilas.SetPosition(23, 191);
	katilas.SetSize(123, 201);
	katilas.SetVectorBytes((uint8_t*)g_katilas);
	katilas.SetUIID("Katilas");

	ktemp.SetCenterXPosition(84, 290);
	ktemp.SetUIID("ktemp");

	boilert_virsus.SetCenterXPosition(288, 50);
	boilert_virsus.SetUIID("boiler_virsus");

	chimneytemp.SetPosition(78, 72);
	chimneytemp.textAlign = AlignLeft;
	chimneytemp.SetUIID("chimneyt");

	ktemp2.SetCenterXPosition(194, 247);
	ktemp2.SetUIID("ktemp2");

	cirk1.SetPosition(192, 28);
	cirk1.SetSize(41, 41);
	cirk1.SetRotation(1);
	cirk1.SetUIID("cirk_boilerio");

	boileris.SetPosition(241, 12);
	boileris.SetSize(94, 182);
	boileris.SetGraphicsBytes((uint8_t*)bmp_boileris);
	boileris.SetUIID("boileris");

	boilert_apacia.SetCenterXPosition(288, 140);
	boilert_apacia.SetUIID("boiler_apacia");

	ladom.SetPosition(225, 295);
	ladom.SetUIID("Ladomatas");

	trisakis.SetPosition(233, 210);
	trisakis.SetSize(41, 30);
	trisakis.SetVectorBytes((uint8_t*)g_trisakis);
	trisakis.SetUIID("Trisakis");

	akum.SetPosition(378, 150);
	akum.SetSize(130, 245);
	akum.SetVectorBytes((uint8_t*)g_akumuliacine);
	akum.SetUIID("akumulc");

	akumt1.SetCenterXPosition(464, 160);
	akumt1.SetUIID("akumt1");

	akumt2.SetCenterXPosition(464, 259);
	akumt2.SetUIID("akumt2");

	akumt3.SetCenterXPosition(464, 359);
	akumt3.SetUIID("akumt3");

	tempbar.SetPosition(385, 160);
	tempbar.SetSize(35, 225);
	tempbar.SetUIID("tempbar");

	kol1.SetPosition(566, 84);
	kol1.SetSize(96, 15);
	kol1.SetVectorBytes((uint8_t*)g_kolektorius);
	kol1.SetUIID("kolekt1");

	grindys.SetPosition(398, 14);
	grindys.SetSize(103, 58);
	grindys.SetGraphicsBytes((uint8_t*)bmp_grindys);
	grindys.SetUIID("grindys");

	cirk2.SetPosition(517, 110);
	cirk2.SetSize(41, 41);
	cirk2.SetRotation(4);
	cirk2.SetUIID("grindu_cirk");

	radiatorius.SetPosition(561, 11);
	radiatorius.SetSize(103, 62);
	radiatorius.SetGraphicsBytes((uint8_t*)bmp_radiatorius);
	radiatorius.SetUIID("radiat");

	kol2.SetPosition(401, 84);
	kol2.SetSize(96, 15);
	kol2.SetVectorBytes((uint8_t*)g_kolektorius);
	kol2.SetUIID("kolekt2");

	cirk3.SetPosition(665, 110);
	cirk3.SetSize(41, 41);
	cirk3.SetRotation(4);
	cirk3.SetUIID("cirk_radiatoriu");

	lbl.SetPosition(getClientArea().Left + 30, getClientArea().Bottom - 30);
	lbl.SetSize(220, 26);
	lbl.SetUIID("infolbl");
	lbl.textAlign = AlignLeft;

	btn.SetPosition(35, 397);
	btn.SetCaption("Ikurimas");
	btn.SetUIID("Batonas");

	char v[64];
	sprintf(v, "UIElements: %d", uielements.Count());
	Log.debug(v);
	Log.debug("ktemp->AttachTempSensor(TSensors.getByName(\"katilas\"));");
	ktemp.AttachTempSensor(TSensors.getByName("katilas"));
	Log.debug("boilert_virsus->AttachTempSensor(TSensors.getByName(\"boileris_virsus\"));");
	boilert_virsus.AttachTempSensor(TSensors.getByName("boileris_virsus"));
	Log.debug("ktemp2->AttachTempSensor(TSensors.getByName(\"arduino\"));");
	ktemp2.AttachTempSensor(TSensors.getByName("arduino"));
	Log.debug("cirk1->AttachPump(CirculationPumps.GetByName(\"boilerio\"));");
	cirk1.AttachPump(CirculationPumps.GetByName("boilerio"));
	Log.debug("boilert_apacia->AttachTempSensor(TSensors.getByName(\"boileris_apacia\"));");
	boilert_apacia.AttachTempSensor(TSensors.getByName("boileris_apacia"));
	Log.debug("akumt1->AttachTempSensor(TSensors.getByName(\"akum_virsus\"));");
	akumt1.AttachTempSensor(TSensors.getByName("akum_virsus"));
	Log.debug("akumt2->AttachTempSensor(TSensors.getByName(\"akum_vidurys\"));");
	akumt2.AttachTempSensor(TSensors.getByName("akum_vidurys"));
	Log.debug("akumt3->AttachTempSensor(TSensors.getByName(\"akum_apacia\"));");
	akumt3.AttachTempSensor(TSensors.getByName("akum_apacia"));
	Log.debug("tempbar->SetTempSensors(TSensors.getByName(\"akum_virsus\"), TSensors.getByName(\"akum_vidurys\"), TSensors.getByName(\"akum_apacia\"));");
	tempbar.SetTempSensors(TSensors.getByName("akum_virsus"), TSensors.getByName("akum_vidurys"), TSensors.getByName("akum_apacia"));
	Log.debug("cirk2->AttachPump(CirculationPumps.GetByName(\"grindu\"));");
	cirk2.AttachPump(CirculationPumps.GetByName("grindu"));
	Log.debug("cirk3->AttachPump(CirculationPumps.GetByName(\"radiatoriu\"));");
	cirk3.AttachPump(CirculationPumps.GetByName("radiatoriu"));

	Log.debug("ktype = KTypes.GetByName(\"dumu_temp\");");
	ktype = KTypes.GetByName("dumu_temp");
	Log.debug("if (ktype)");
	if (ktype)
	{
		Log.debug("chimneytemp->AttachKType(ktype);");
		chimneytemp.AttachKType(ktype);
	}

	Log.debug("woodb = WoodBoilers.GetByName(\"default\");");
	woodb = WoodBoilers.GetByName("default");
	Log.debug("if (woodb)");
	if (woodb)
	{
		ladom.AttachWoodBoiler(woodb);
		ventiliatorius.AttachWoodBoiler(woodb);
		Log.debug("woodb->AddStatusChangeHandler(this);");
		woodb->AddStatusChangeHandler(this);
		Log.debug("HandleStatusChange(woodb, woodb->GetKatilasStatus());");
		HandleStatusChange(woodb, woodb->GetKatilasStatus());
	}

	Log.debug("btn->AddOnClickHandler(this);");
	btn.AddOnClickHandler(this);
	Log.debug("void WindowKatiline::Begin() DONE.");
}

void WindowKatiline::TimeEvent(uint32_t t)
{
}

void WindowKatiline::HandleOnClick(void* Sender, int x, int y, int tag)
{
	if(Sender == &btn)
	{
		//Log.debug("WindowKatilineOnClick: paspaustas buttonas.");
		WoodBoilers.Get(0)->EnterPhase2();
	}
	else if(Sender == &ventiliatorius)
	{
		//Log.debug("WindowKatilineOnClick: paspaustas ventiliatorius.");
		WoodBoilers.Get(0)->ChangeExhaustFanManual();
	}
	else if(Sender == &ladom)
	{
		//Log.debug("WindowKatilineOnClick: paspaustas ladomatas.");
		WoodBoilers.Get(0)->ChangeLadomatManual();
	}
	else
	{
		Log.debug("WindowKatilineOnClick: xer znajet kas atsiunte");
	}
}

void WindowKatiline::HandleStatusChange(void *Sender, Status status)
{
	if (Sender == WoodBoilers.Get(0))
	{
		switch (status)
		{
		case STATUS_UNKNOWN:
		case STATUS_KATILAS_UNKNOWN:
			lbl.SetCaption("Busena tikrinama");
			break;
		case STATUS_KATILAS_UZGESES:
			lbl.SetCaption("Uzgeses");
			break;
		case STATUS_KATILAS_IKURIAMAS:
			lbl.SetCaption("Ikurimas");
			break;
		case STATUS_KATILAS_KURENASI:
			lbl.SetCaption("Kurenasi");
			break;
		case STATUS_KATILAS_GESTA:
			lbl.SetCaption("Gesta");
			break;
		default:
			break;
		}
	}
	else
	{
		char s[128];
		snprintf(s, 128, "Handling WindowKatiline::HandleStatusChange - Atsiunte ne Katilas!!! Status: %s", Events::GetStatusName(status));
		Log.debug(s);
	}
}
