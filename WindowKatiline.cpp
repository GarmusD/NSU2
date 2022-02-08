#include "WindowKatiline.h"
#include "graphics.h"
//#include "bmp_saule.h"
#include "TSensors.h"

WindowKatiline::WindowKatiline(Rect windowarea, UTFT& utft, Scenario& Sc, byte* sc_buf):CWindow(windowarea, utft, Sc, sc_buf)
{
	WoodBoilers.Get(0)->AddStatusChangeHandler(this);
	kstatus = WoodBoilers.Get(0)->GetKatilasStatus();
	vent_on = WoodBoilers.Get(0)->IsExhaustFanOn();
	vent_manual = WoodBoilers.Get(0)->IsExhaustFanManual();
	ladom_on = WoodBoilers.Get(0)->IsLadomatasOn();

	MAX31855* ktype = KTypes.GetByName("dumu_temp");
	Log.debug("kaminas = new UIGraphics(this);");
	kaminas = new UIGraphics(this);
	kaminas->SetSize(41, 122);
	kaminas->SetPosition(41, 12);
	kaminas->SetVectorBytes((byte*)g_kaminas);
	kaminas->SetUIID("Kaminas");

	
	Log.debug("ventiliatorius = new UIVentilator(this);");
	ventiliatorius = new UIVentilator(this);
	ventiliatorius->SetPosition(45, 145);
	ventiliatorius->SetUIID("vent");

	Log.debug("katilas = new UIGraphics(this);");
	katilas = new UIGraphics(this);
	katilas->SetPosition(23, 191);
	katilas->SetSize(123, 201);
	katilas->SetVectorBytes((byte*)g_katilas);
	katilas->SetUIID("Katilas");

	Log.debug("uitl = new UITempLabel(this);");
	ktemp = new UITempLabel(this);
	ktemp->SetCenterXPosition(84, 290);
	ktemp->SetUIID("ktemp");
	ktemp->AttachTempSensor(TSensors.getByName("katilas"));

	Log.debug("boilert_virsus = new UITempLabel(this);");
	boilert_virsus = new UITempLabel(this);
	boilert_virsus->SetCenterXPosition(288, 50);
	boilert_virsus->SetUIID("boiler_virsus");
	boilert_virsus->AttachTempSensor(TSensors.getByName("boileris_virsus"));

	if (ktype != NULL) {
		Log.debug("chimneytemp = new UITempLabel(this);");
		chimneytemp = new UITempLabel(this);
		Log.debug("chimneytemp->SetPosition(78, 72);");
		chimneytemp->SetPosition(78, 72);
		Log.debug("chimneytemp->textAlign = AlignLeft;");
		chimneytemp->textAlign = AlignLeft;
		Log.debug("chimneytemp->SetUIID(chimneyt);");
		chimneytemp->SetUIID("chimneyt");
		Log.debug("chimneytemp->AttachKType(ktype);");
		chimneytemp->AttachKType(ktype);
	}

	Log.debug("ktemp2 = new UITempLabel(this);");
	ktemp2 = new UITempLabel(this);
	ktemp2->SetCenterXPosition(194, 247);
	ktemp2->SetUIID("ktemp2");
	ktemp2->AttachTempSensor(TSensors.getByName("arduino"));
	Log.debug("cirk1 = new UICircPump(this);");
	cirk1 = new UICircPump(this);
	cirk1->SetPosition(192, 28);
	cirk1->SetSize(41, 41);
	cirk1->SetRotation(1);
	cirk1->SetUIID("cirk_boilerio");
	cirk1->AttachPump(CirculationPumps.GetByName("boilerio"));
	Log.debug("boileris = new UIGraphics(this);");
	boileris = new UIGraphics(this);
	boileris->SetPosition(241, 12);
	boileris->SetSize(94, 182);
	boileris->SetGraphicsBytes((byte*)bmp_boileris);
	boileris->SetUIID("boileris");

	

	Log.debug("boilert_apacia = new UITempLabel(this);");
	boilert_apacia = new UITempLabel(this);
	boilert_apacia->SetCenterXPosition(288, 140);
	boilert_apacia->SetUIID("boiler_apacia");
	boilert_apacia->AttachTempSensor(TSensors.getByName("boileris_apacia"));
	Log.debug("ladom = new UILadomat(this);");
	ladom = new UILadomat(this);
	ladom->SetPosition(225, 295);
	//ladom->SetSize(61, 85);
	//ladom->SetGraphicsBytes((byte*)bmp_ladomatas);
	ladom->SetUIID("Ladomatas");
	//ladom->AddOnClickHandler(this);
	//if(ladom_on){
	//	ladom->SetColor(CP_ON_Color);
	//}else{
	//	ladom->SetColor(CP_OFF_Color);
	//}
	Log.debug("trisakis = new UIGraphics(this);");
	trisakis = new UIGraphics(this);
	trisakis->SetPosition(233, 210);
	trisakis->SetSize(41, 30);
	trisakis->SetVectorBytes((byte*)g_trisakis);
	trisakis->SetUIID("Trisakis");
	Log.debug("akum = new UIGraphics(this);");
	akum = new UIGraphics(this);
	akum->SetPosition(378, 150);
	akum->SetSize(130, 245);
	akum->SetVectorBytes((byte*)g_akumuliacine);
	akum->SetUIID("akumulc");
	Log.debug("akumt1 = new UITempLabel(this);");
	akumt1 = new UITempLabel(this);
	akumt1->SetCenterXPosition(464, 160);
	akumt1->SetUIID("akumt1");
	akumt1->AttachTempSensor(TSensors.getByName("akum_virsus"));
	Log.debug("akumt2 = new UITempLabel(this);");
	akumt2 = new UITempLabel(this);
	akumt2->SetCenterXPosition(464, 259);
	akumt2->SetUIID("akumt2");
	akumt2->AttachTempSensor(TSensors.getByName("akum_vidurys"));
	Log.debug("akumt3 = new UITempLabel(this);");
	akumt3 = new UITempLabel(this);
	akumt3->SetCenterXPosition(464, 359);
	akumt3->SetUIID("akumt3");
	akumt3->AttachTempSensor(TSensors.getByName("akum_apacia"));

	Log.debug("tempbar = new UITempBar(this);");
	tempbar = new UITempBar(this);
	tempbar->SetPosition(385, 160);
	tempbar->SetSize(35, 225);
	tempbar->SetUIID("tempbar");
	tempbar->SetTempSensors(TSensors.getByName("akum_virsus"), TSensors.getByName("akum_vidurys"), TSensors.getByName("akum_apacia"));
	Log.debug("kol1 = new UIGraphics(this);");
	kol1 = new UIGraphics(this);
	kol1->SetPosition(566, 84);
	kol1->SetSize(96, 15);
	kol1->SetVectorBytes((byte*)g_kolektorius);
	kol1->SetUIID("kolekt1");
	Log.debug("grindys = new UIGraphics(this);");
	grindys = new UIGraphics(this);
	grindys->SetPosition(398, 14);
	grindys->SetSize(103, 58);
	grindys->SetGraphicsBytes((byte*)bmp_grindys);
	grindys->SetUIID("grindys");
	Log.debug("cirk2 = new UICircPump(this);");
	cirk2 = new UICircPump(this);
	cirk2->SetPosition(517, 110);
	cirk2->SetSize(41, 41);
	cirk2->SetRotation(4);
	cirk2->SetUIID("grindu_cirk");
	cirk2->AttachPump(CirculationPumps.GetByName("grindu"));
	Log.debug("radiatorius = new UIGraphics(this);");
	radiatorius = new UIGraphics(this);
	radiatorius->SetPosition(561, 11);
	radiatorius->SetSize(103, 62);
	radiatorius->SetGraphicsBytes((byte*)bmp_radiatorius);
	radiatorius->SetUIID("radiat");
	Log.debug("kol2 = new UIGraphics(this);");
	kol2 = new UIGraphics(this);
	kol2->SetPosition(401, 84);
	kol2->SetSize(96, 15);
	kol2->SetVectorBytes((byte*)g_kolektorius);
	kol2->SetUIID("kolekt2");
	Log.debug("cirk3 = new UICircPump(this);");
	cirk3 = new UICircPump(this);
	cirk3->SetPosition(665, 110);
	cirk3->SetSize(41, 41);
	cirk3->SetRotation(4);
	cirk3->SetUIID("cirk_radiatoriu");
	cirk3->AttachPump(CirculationPumps.GetByName("radiatoriu"));
	Log.debug("lbl = new UILabel(this);");
	lbl = new UILabel(this);
	lbl->SetPosition(getClientArea().Left + 30, getClientArea().Bottom - 30);
	lbl->SetSize(220, 26);
	lbl->SetUIID("infolbl");
	lbl->textAlign = AlignLeft;
	lbl->SetCaption(GetKatilasStatusText(kstatus));
	Log.debug("btn = new UIButton(this);");
	btn = new UIButton(this);
	btn->SetPosition(35, 397);
	btn->SetCaption("Ikurimas");
	btn->SetUIID("Batonas");
	btn->AddOnClickHandler(this);
	Log.debug("Katiline Constructor done.");
}


WindowKatiline::~WindowKatiline(void)
{
	WoodBoilers.Get(0)->RemoveStatusChangeHandler(this);
}

void WindowKatiline::TimeEvent(uint32_t t)
{
}

void WindowKatiline::HandleOnClick(void* Sender, int x, int y, int tag)
{
	if(Sender == btn){
		Log.debug("WindowKatilineOnClick: paspaustas buttonas.");
		WoodBoilers.Get(0)->EnterPhase2();
	}else if(Sender == ventiliatorius){
		Log.debug("WindowKatilineOnClick: paspaustas ventiliatorius.");
		WoodBoilers.Get(0)->ChangeExhaustFanManual();
	}
	else if(Sender == ladom){
		Log.debug("WindowKatilineOnClick: paspaustas ladomatas.");
		WoodBoilers.Get(0)->ChangeLadomatManual();
	
	}else{
		Log.debug("WindowKatilineOnClick: xer znajet kas atsiunte");
	}
}

void WindowKatiline::HandleStatusChange(void *Sender, Status status){
	if (Sender == WoodBoilers.Get(0)){
		switch (status)
		{
		case STATUS_UNKNOWN:
			break;
		case STATUS_OFF:
			break;
		case STATUS_ON:
			break;
		case STATUS_MANUAL:
			break;
		case STATUS_DISABLED:
			break;
		case STATUS_DISABLED_OFF:
			break;
		case STATUS_DISABLED_ON:
			break;
		case STATUS_KATILAS_UNKNOWN:
		case STATUS_KATILAS_UZGESES:
		case STATUS_KATILAS_IKURIAMAS:
		case STATUS_KATILAS_KURENASI:
		case STATUS_KATILAS_GESTA:
			Log.debug("Handling WindowKatiline::HandleStatusChange.");
			lbl->SetCaption(GetKatilasStatusText(status));
			break;
			/*
		case STATUS_LADOMAT_OFF:
		ladom->SetColor(CP_OFF_Color);
		ladom->Invalidate();
		break;
		case STATUS_LADOMAT_ON:
		ladom->SetColor(CP_ON_Color);
		ladom->Invalidate();
		break;
		case STATUS_LADOMAT_MANUAL:
		ladom->SetColor(CP_Manual_Color);
		ladom->Invalidate();
		break;
		case STATUS_VENT_OFF:
		ventiliatorius->SetColor(CP_OFF_Color);
		ventiliatorius->Invalidate();
		break;
		case STATUS_VENT_ON:
		ventiliatorius->SetColor(CP_ON_Color);
		ventiliatorius->Invalidate();
		break;
		case STATUS_VENT_MANUAL:
		ventiliatorius->SetColor(CP_Manual_Color);
		ventiliatorius->Invalidate();
		break;
		*/
		default:
			break;
		}
	}
	else
	{
		Log.debug("Handling WindowKatiline::HandleStatusChange - Atsiunte ne Katilas!!! Status: " +Events::GetStatusName(status));
	}
}

String WindowKatiline::GetKatilasStatusText(Status value){
	switch (value)
	{
	case STATUS_KATILAS_UNKNOWN:
		return "Busena tikrinama";
		break;
	case STATUS_KATILAS_UZGESES:
		return "Uzgeses";
		break;
	case STATUS_KATILAS_IKURIAMAS:
		return "Ikurimas";
		break;
	case STATUS_KATILAS_KURENASI:
		return "Kurenasi";
		break;
	case STATUS_KATILAS_GESTA:
		return "Gesta";
		break;
	default:
		break;
	}
	return "error";
}