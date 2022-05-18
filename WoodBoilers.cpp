#include "WoodBoilers.h"

const int CWoodBoilers::CURRENT_CONFIG_VERSION = 3;
const VersionInfo CWoodBoilers::VINFO = { 'W', 'D', 'B', CURRENT_CONFIG_VERSION };
const char* CWoodBoilers::CFG_FILE = "woodblr.cfg";


CWoodBoiler::CWoodBoiler(void)
{
	Reset();
}

CWoodBoiler::~CWoodBoiler(void)
{
}

void CWoodBoiler::Reset()
{
	Events::Reset();
	opmodeswitch = NULL;
	burnmodeswitch = NULL;
	waterboiler = NULL;
	opmode = WinterMode;
	burnmode = EcoMode;

	simulation = false;
	sim_orig_temp = 0.0;

	strlcpy(name, "default", MAX_NAME_LENGTH);
	status = STATUS_KATILAS_UNKNOWN;
	exhaustfan_status = STATUS_EXHAUSTFAN_OFF;
	ladomat_status = STATUS_LADOMAT_OFF;
	lastcheck = 0;
	temp_array_check = 0;
	working_temp = 85.0;
	histeresis = 2.0;
	chimneytemp = 20.0;
	watertemp = 20.0;
	avgwatertemp = 20.0;
	ladomattemp = 60.0;
	ladomat_trigger = NULL;
	ladomat_trigger_status = STATUS_OFF;
	chimneytempsens = NULL;
	chimneytem_enabled = false;
	ladom_channel = 0xFF;
	exhaustfan_channel = 0xFF;
	ladom_on = false;
	exhaust_on = false;
	exhaust_manual = false;
	ladom_manual = false;
	exhaust_man_start = 0;
	ladom_man_start = 0;

	exhaust_disabled = false;
	exhaust_ctrl_enabled = false;
	exhaust_last_state = false;

	temp_status = TS_Stable;
	temp_array_add = true;
	for(uint8_t i=0; i < TEMP_ARRAY_SIZE; i++){
		temp_array[i] = -55.5;
	}	
	katilasInfoTime = 0;

	opmodeswitch = NULL;
	burnmodeswitch = NULL;

	StartingPhase();
}

void CWoodBoiler::Begin()
{
	TimeSlice.RegisterTimeSlice(this);

	opmodeswitch = Switches.GetByName(SWITCH_NAMES[0]);
	if (opmodeswitch != NULL)
	{
		opmodeswitch->AddStatusChangeHandler(this);
		HandleStatusChange(opmodeswitch, opmodeswitch->GetStatus());
	}
	burnmodeswitch = Switches.GetByName(SWITCH_NAMES[3]);
	//burnmodeswitch->AddStatusChangeHandler(this);
}

bool CWoodBoiler::SetSimulationMode(bool value)
{
	if(value && watertemp >= working_temp + 2.0)
	{
		char s[64];
		snprintf(s, 64, "Simulation disabled because water_temp is too high: %.1f", watertemp);
		Log.debug(s);
		return false;
	}
	if(value && !simulation)
	{
		simulation = true;
		sim_start = millis();
		sim_orig_temp = watertemp;
		return true;
	}
	else if(!value && simulation)
	{
		simulation = false;
		watertemp = sim_orig_temp;
		DoBoilerRoutine();
		return true;
	}
	Log.error("Unknown simulation error.");
	return false;
}

bool CWoodBoiler::SetSimulationTemp(float value)
{
	if(!simulation)
	{
		Log.debug("Not in simulation mode!");
		return false;
	}
	sim_start = millis();
	watertemp = value;
	DoBoilerRoutine();
	return true;
}

void CWoodBoiler::OnTimeSlice()//:TimeEvent(uint32_t t)
{
	//Log.debug("KatilasClass::OnTimeSlice()");
	if(simulation && Utilities::CheckMillis(sim_start, MINUTES10, false))
	{
		SetSimulationMode(false);
	}

	if(Utilities::CheckMillis(temp_array_check, 60000) || temp_array_check == 0)
	{
		AddTempArray(watertemp);
	}
	
	if(Utilities::CheckMillis(exhaust_man_start, MINUTES5 * 3, false) && exhaust_manual){
		ChangeExhaustFanManual();
	}

	if(Utilities::CheckMillis(ladom_man_start, MINUTES5 * 3, false) && ladom_manual){
		ChangeLadomatManual();
	}

	if(Utilities::CheckMillis(lastcheck, 30000))
	{
		DoBoilerRoutine();		
	}
}

void CWoodBoiler::DoBoilerRoutine()
{
	switch (status)
		{
		case STATUS_KATILAS_UNKNOWN:
			Phase0();
			break;
		case STATUS_KATILAS_IKURIAMAS:
			Phase2();
			break;
		case STATUS_KATILAS_KURENASI:
			Phase3();
			break;
		case STATUS_KATILAS_GESTA:
			Phase4();
			break;
		case STATUS_KATILAS_UZGESES:
			Phase1();
			break;
		default:
			char s[128];
			sprintf(s, "KatilasClass::DoBoilerRoutine() - Nezinoma bukle!!! Status: %s", Events::GetStatusName(status));
			Log.debug(s);
			break;
		}
}

void CWoodBoiler::HandleTemperatureChange(void* Sender, float temp)
{
	if(Sender == watertempsens)
	{
		if(temp != -127.0)
		{
			if(simulation)
			{
				sim_orig_temp = temp;
				if(temp >= working_temp + 3.0)//real temp is over safe range
				{
					SetSimulationMode(false);
				}
			}
			else
			{
				watertemp = temp;
				CAlarm* al = Alarms.GetByConfigPos(0);
				if (al)
					al->SetCurrentTemperature(watertemp);
			}
			lastcheck = millis();
			DoBoilerRoutine();
		}
	}
	else if(Sender == chimneytempsens)
	{
		chimneytemp = temp;
		if (exhaust_ctrl_enabled)
		{
			if (chimneytemp >= 260.0f && !exhaust_disabled)
			{				
				VentOff();
				exhaust_disabled = true;
			}
			if (chimneytemp <= 200.0f && exhaust_disabled)
			{
				exhaust_disabled = false;
				if (exhaust_last_state)
				{
					VentOn();
				}
			}
		}
	}
}

void CWoodBoiler::HandleStatusChange(void* Sender, Status status)
{
	if(Sender == opmodeswitch)
	{
		if(status == STATUS_ON || status == STATUS_DISABLED_ON)
		{
			SwitchOperationMode(WinterMode);
		}
		else if(status == STATUS_OFF || status == STATUS_DISABLED_OFF)
		{
			SwitchOperationMode(SummerMode);
		}
	}
	else if(Sender == burnmodeswitch)
	{
		if(status == STATUS_ON || status == STATUS_DISABLED_ON)
		{
			SwitchBurnMode(EcoMode);
		}
		else if(status == STATUS_OFF || status == STATUS_DISABLED_OFF)
		{
			SwitchBurnMode(NormalMode);
		}
	}
	else if (Sender == ladomat_trigger && Sender != NULL)
	{
		ladomat_trigger_status = status;
		if (ladomat_trigger_status == STATUS_OFF && this->status == STATUS_KATILAS_GESTA && IsLadomatasOn())
		{
			LadomOff();
		}
		else if (ladomat_trigger_status == STATUS_ON && this->status == STATUS_KATILAS_GESTA && !IsLadomatasOn())
		{
			LadomOn();
		}
	}
}

void CWoodBoiler::SwitchOperationMode(OperationMode new_mode)
{
	Log.debug("Katilas: Switching OperationMode");
	if(new_mode != opmode)
	{
		if(new_mode == SummerMode)
		{
			if(waterboiler)
				waterboiler->SetExternalControl(true);
		}
		else if(new_mode == WinterMode)
		{
			if (waterboiler)
			{
				waterboiler->StopCircPump();
				waterboiler->SetExternalControl(false);
			}
		}
		opmode = new_mode;
		DoBoilerRoutine();
	}
}

void CWoodBoiler::SwitchBurnMode(BurnMode new_mode)
{
}

void CWoodBoiler::AddTempArray(float value){
	if(temp_array[TEMP_ARRAY_SIZE-1] == -55.5){//starting
		for(uint8_t i=0; i<TEMP_ARRAY_SIZE; i++){
			temp_array[i] = value;
		}
	}else{
		for(uint8_t i=1; i < TEMP_ARRAY_SIZE; i++){
			temp_array[i-1] = temp_array[i];
		}
		temp_array[TEMP_ARRAY_SIZE-1] = value;
	}
	//calculate temp_status
	float t1 = 0;
	float t2 = 0;
	uint8_t c1 = 0;
	uint8_t c2 = 0;

	for(uint8_t i=0; i < TEMP_ARRAY_SIZE / 2; i++){
		float t = temp_array[i];
			t1 += t;
			c1++;
	}

	for(uint8_t i=TEMP_ARRAY_SIZE / 2; i < TEMP_ARRAY_SIZE; i++){
		float t = temp_array[i];
			t2 += t;
			c2++;
	}

	if(t1 == -55.5){
		t1 = t2;
		c1 = c2;
	}

	float avg1 = t1 / (float)c1;
	float avg2 = t2 / (float)c2;

	if (avg1 < avg2){
		temp_status = TS_Growing;
	}else if(avg1 > avg2){
		temp_status = TS_Lowering;
	}else{
		temp_status = TS_Stable;
	}
}

void CWoodBoiler::SetName(const char* n)
{
	strlcpy(name, n, MAX_NAME_LENGTH);
}

const char* CWoodBoiler::GetName()
{
	return name;
}

void CWoodBoiler::SetTempSensor(TempSensor* sensor)
{
	if (watertempsens)
	{
		watertempsens->RemoveTempChangeHandler(this);
	}
	watertempsens = sensor;
	if(watertempsens)
	{
		char s[64];
		snprintf(s, 64, "Katilas TSensor set: %s", sensor->getName());
		Log.debug(s);
		watertempsens->AddTempChangeHandler(this);
		HandleTemperatureChange(watertempsens, watertempsens->getTemp());
	}
}

void CWoodBoiler::SetKTypeSensor(MAX31855* sensor)
{
	if (chimneytempsens)
	{
		RemoveStatusChangeHandler(chimneytempsens);
		chimneytempsens->RemoveTempChangeHandler(this);
	}
	chimneytempsens = sensor;
	if(chimneytempsens)
	{
		char s[64];
		snprintf(s, 64, "Katilas KType Sensor set: %s", sensor->GetName());
		Log.debug(s);
		chimneytempsens->AddTempChangeHandler(this);
		chimneytemp = chimneytempsens->GetTemp();
		chimneytem_enabled = true;
		AddStatusChangeHandler(chimneytempsens);
		DispatchStatusChange(this, status); // inaccessible -> chimneytempsens->HandleStatusChange(this, status);
	}else{
		chimneytem_enabled = false;
	}
}

void CWoodBoiler::EnableChimneyTemp(bool enable)
{
	chimneytem_enabled = enable;
}

void CWoodBoiler::SetWaterBoiler(CWaterBoiler* b)
{
	waterboiler = b;
}

void CWoodBoiler::SetLadomatChannel(uint8_t idx)
{
	ladom_channel = idx;
	if(ladom_on && ladom_channel != 0xFF){		
		ladomat_status = STATUS_LADOMAT_ON;
		PrintLadomatStatus();
		DispatchStatusChange(this, ladomat_status);
		RelayModules.OpenChannel(ladom_channel);
	}
}

void CWoodBoiler::SetExhaustFanChannel(uint8_t idx)
{
	exhaustfan_channel = idx;
	if(exhaust_on && exhaustfan_channel != 0xFF){		
		exhaustfan_status = STATUS_EXHAUSTFAN_ON;
		PrintExhaustFanStatus();
		DispatchStatusChange(this, exhaustfan_status);
		RelayModules.OpenChannel(exhaustfan_channel);
	}
}

float CWoodBoiler::GetCurrentTemp()
{
	return watertemp;
}

void CWoodBoiler::SetWorkingTemp(float work_temp)
{
	working_temp = work_temp;
}

void CWoodBoiler::SetHisteresis(float temp)
{
	histeresis = temp;
}

void CWoodBoiler::SetLadomatTemp(float ltemp)
{
	ladomattemp = ltemp;
}

void CWoodBoiler::SetLadomatTrigger(TempTrigger* ltrigger)
{
	if (ladomat_trigger != NULL)
	{
		ladomat_trigger->RemoveStatusChangeHandler(this);
	}
	ladomat_trigger = ltrigger;
	if (ladomat_trigger != NULL)
	{
		ladomat_trigger->AddStatusChangeHandler(this);
		HandleStatusChange(ladomat_trigger, ladomat_trigger->GetStatus());
	}
}

void CWoodBoiler::SetOperationModeSwitch(Switch* op_mode){
	if(op_mode != NULL)
	{
		opmodeswitch = op_mode;
		opmodeswitch->AddStatusChangeHandler(this);
		HandleStatusChange(opmodeswitch, opmodeswitch->GetStatus());
	}
}

void CWoodBoiler::SetBurnModeSwitch(Switch* burn_mode){
	if(burn_mode != NULL)
	{
		burnmodeswitch = burn_mode;
		burnmodeswitch->AddStatusChangeHandler(this);
		HandleStatusChange(burnmodeswitch, burnmodeswitch->GetStatus());
	}
}

Status CWoodBoiler::GetKatilasStatus(){
	return status;
}

Status CWoodBoiler::GetLadomatStatus()
{
	return ladomat_status;
}

Status CWoodBoiler::GetSmokeFanStatus()
{
	return exhaustfan_status;
}

bool CWoodBoiler::IsLadomatasOn(){
	if(ladom_manual) return true;
	return ladom_on;
}

bool CWoodBoiler::IsLadomatManual()
{
	return ladom_manual;
}

bool CWoodBoiler::IsExhaustFanOn(){
	if(exhaust_manual) return true;
	return exhaust_on;
}

bool CWoodBoiler::IsExhaustFanManual(){
	return exhaust_manual;
}

void CWoodBoiler::ChangeExhaustFanManual(){
	//Serial.println("DBG: Katilas:ChangeVentilatorManual().");
	if(exhaust_manual){//stop vent
		exhaust_manual = false;
		Log.debug("Vent manual OFF");
		if(!exhaust_on && exhaustfan_channel != 0xFF){			
			ChangeExhaustFanStatus(STATUS_EXHAUSTFAN_OFF);
			RelayModules.CloseChannel(exhaustfan_channel);
		}
		else
		{
			ChangeExhaustFanStatus(STATUS_EXHAUSTFAN_ON);
		}
	}else{//start vent
		exhaust_man_start = millis();
		exhaust_manual = true;		
		ChangeExhaustFanStatus(STATUS_EXHAUSTFAN_MANUAL);
		if (!exhaust_on && exhaustfan_channel != 0xFF) {
			RelayModules.OpenChannel(exhaustfan_channel);
		}
	}
}

void CWoodBoiler::ChangeLadomatManual(){
	//Serial.println("DBG: Katilas:ChangeVentilatorManual().");
	if(ladom_manual){//stop ladom
		ladom_manual = false;
		Log.debug("Ladomat manual OFF");
		if(!ladom_on && ladom_channel != 0xFF){			
			ChangeLadomatStatus(STATUS_LADOMAT_OFF);
			RelayModules.CloseChannel(ladom_channel);
		}
		else
		{
			ChangeLadomatStatus(STATUS_LADOMAT_ON);
		}
	}else{//start ladom
		ladom_man_start = millis();
		ladom_manual = true;		
		ChangeLadomatStatus(STATUS_LADOMAT_MANUAL);
		if (!ladom_on && ladom_channel != 0xFF) {
			RelayModules.OpenChannel(ladom_channel);
		}
	}
}

void CWoodBoiler::LadomOn()
{	
	if(ladom_on) return;
	ladom_on = true;
	if(ladom_channel != 0xFF)
	{		
		ChangeLadomatStatus(STATUS_LADOMAT_ON);
		RelayModules.OpenChannel(ladom_channel);		
	}
}

void CWoodBoiler::LadomOff()
{	
	if(!ladom_on) return;
	exhaust_last_state = false;
	ladom_on = false;
	if(ladom_channel != 0xFF && !ladom_manual)
	{
		ChangeLadomatStatus(STATUS_LADOMAT_OFF);
		RelayModules.CloseChannel(ladom_channel);
	}
}

void CWoodBoiler::VentOn()
{
	exhaust_last_state = true;
	if (exhaust_disabled) return;
	if(exhaust_on) return;
	exhaust_on = true;
	exhaust_start = millis();
	if(exhaustfan_channel != 0xFF)
	{	
		ChangeExhaustFanStatus(STATUS_EXHAUSTFAN_ON);
		RelayModules.OpenChannel(exhaustfan_channel);
	}
}

void CWoodBoiler::VentOff()
{
	if (exhaust_disabled) return;
	if(!exhaust_on) return;
	exhaust_on = false;
	if(exhaustfan_channel != 0xFF && !exhaust_manual)
	{
		ChangeExhaustFanStatus(STATUS_EXHAUSTFAN_OFF);
		RelayModules.CloseChannel(exhaustfan_channel);
	}
}

void CWoodBoiler::ChangeExhaustFanStatus(Status newStatus)
{
	exhaustfan_status = newStatus;
	PrintExhaustFanStatus();
	DispatchStatusChange(this, exhaustfan_status);
}

void CWoodBoiler::ChangeLadomatStatus(Status newStatus)
{
	ladomat_status = newStatus;
	PrintLadomatStatus();
	DispatchStatusChange(this, ladomat_status);
}

void CWoodBoiler::ChangeWoodBoilerStatus(Status newStatus)
{
	status = newStatus;
	PrintWoodBoilerStatus();
	DispatchStatusChange(this, status);
}


void CWoodBoiler::StartingPhase()
{
	phase0start = millis();
	status = STATUS_KATILAS_UNKNOWN;
	DispatchStatusChange(this, status);
}

void CWoodBoiler::EnterPhase1()//Uzgeses
{
	if(opmode == SummerMode)
	{
		if (waterboiler)
		{
			waterboiler->StopCircPump();
			waterboiler->SetExternalControl(false);
		}
	}
	//Isjungiam ladomata ir ventiliatoriu
	LadomOff();
	VentOff();
	ChangeWoodBoilerStatus(STATUS_KATILAS_UZGESES);
}

//Ikuriamas
void CWoodBoiler::EnterPhase2()
{
	phase2start = millis();
	if(opmode == WinterMode)
	{
		LadomOn();
	}
	else if(opmode == SummerMode)
	{
		if(watertemp < ladomattemp)
		{
			LadomOn();
			if (waterboiler)
			{
				waterboiler->StopCircPump();
				waterboiler->SetExternalControl(false);
			}
		}
		else if(ladom_on)
		{
			LadomOff();
			if (waterboiler)
			{
				waterboiler->SetExternalControl(true);
			}
		}
	}
	//Jungiam ventiliatoriu
	if(exhaust_manual) exhaust_manual = false;
	VentOn();

	ChangeWoodBoilerStatus(STATUS_KATILAS_IKURIAMAS);

}

void CWoodBoiler::EnterPhase3()//Kurenasi
{
	if(opmode == WinterMode)
	{
		if (ladom_manual)
		{
			ChangeLadomatManual();
		}
		if(!ladom_on && ladom_channel != 0xFF)
		{			
			LadomOn();
		}
	}
	else if(opmode == SummerMode)
	{
		if(watertemp < ladomattemp)
		{
			LadomOn();
			if (waterboiler)
			{
				waterboiler->SetExternalControl(false);
			}
		}
		else if(ladom_on)
		{
			LadomOff();
			if (waterboiler)
			{
				waterboiler->SetExternalControl(true);
				waterboiler->StartCircPump();
			}
		}
	}

	if(watertemp >= working_temp && exhaust_on)
	{
		VentOff();
	}

	ChangeWoodBoilerStatus(STATUS_KATILAS_KURENASI);
}

void CWoodBoiler::EnterPhase4()//Gesta
{
	phase4start = millis();
	if(opmode == SummerMode)
	{
		if (waterboiler)
		{
			waterboiler->StopCircPump();
			waterboiler->SetExternalControl(false);
		}
	}
	//Isjungiam ladomata ir ventiliatoriu
	if (ladomat_trigger_status == STATUS_OFF && IsLadomatasOn())
	{
		LadomOff();
	}
	else if (ladomat_trigger_status == STATUS_ON && !IsLadomatasOn())
	{
		LadomOn();
	}
	VentOff();
	ChangeWoodBoilerStatus(STATUS_KATILAS_GESTA);
}

void CWoodBoiler::Phase0()//Unknown
{
	if(watertemp >= (working_temp - histeresis))
	{
		EnterPhase3();//Kurenasi
		return;
	}

	if(watertemp >= 30.0 && temp_status == TS_Growing)
	{
		EnterPhase2();//Ikuriamas
		return;
	}

	if(watertemp < 30.0 && (temp_status == TS_Stable || temp_status == TS_Lowering))
	{
		EnterPhase1();//Uzgeses
		return;
	}

	if(watertemp < ladomattemp && temp_status == TS_Lowering)
	{
		EnterPhase4();//Gesta
	}
}

void CWoodBoiler::Phase1()//Uzgeses
{
	if(watertemp >= 30.0 && temp_status == TS_Growing)
	{
		EnterPhase2();
	}
}

void CWoodBoiler::Phase2()//Ikuriamas
{
	if(watertemp >= ladomattemp && temp_status == TS_Growing)
	{
		EnterPhase3();//Kurenasi
		return;
	}

	if(watertemp < 30.0 && (millis()-phase2start > MINUTES5*3) && temp_status != TS_Growing)
	{
		//Per 15 minuciu temp nekyla
		EnterPhase1();
		return;
	}

	if(watertemp < ladomattemp - (histeresis * 3.0) && temp_status == TS_Lowering && (millis()-phase2start > MINUTES5*3))
	{
		//vistik gestam po 15 min
		EnterPhase4();
		return;
	}

	if(opmode == SummerMode)
	{
		if(watertemp < ladomattemp && !ladom_on)
		{
			LadomOn();
		}
		else if(watertemp >= ladomattemp && ladom_on)
		{
			LadomOff();
		}

		if(watertemp >= ladomattemp + 5.0)
		{
			if (waterboiler)
			{
				waterboiler->SetExternalControl(true);
				waterboiler->StartCircPump();
			}
		}
		else if(watertemp <= ladomattemp + 2.0)
		{
			if (waterboiler)
			{
				waterboiler->StopCircPump();
			}
		}
	}
	else if(opmode == WinterMode)
	{
		if(!ladom_on)
		{
			LadomOn();
		}
	}
}

void CWoodBoiler::Phase3()//Kurenasi
{
	if(watertemp <= (working_temp - histeresis) && !exhaust_on)//jungiam ventiliatoriu
	{		
		VentOn();
	}

	if(watertemp >= working_temp && exhaust_on)//isjungiam ventiliatoriu
	{		
		VentOff();
	}

	if(watertemp < ladomattemp && (millis()-exhaust_start > MINUTES5*2) && temp_status == TS_Lowering)//gestam, jei po 10min ventiliatoriaus veikimo, temp krenta
	{		
		EnterPhase4();
		return;
	}

	if(chimneytem_enabled)
	{
		if(chimneytemp < (watertemp + 30) && exhaust_on && temp_status == TS_Lowering)
		{
			EnterPhase4();
			return;
		}
	}

	if(opmode == SummerMode)
	{
		if((watertemp >= working_temp + 5.0) && !ladom_on)
		{
			LadomOn();
		}
		else if(watertemp <= working_temp && ladom_on)
		{
			LadomOff();
		}
	}
	else if (opmode == WinterMode)
	{
		if (!ladom_on)
		{
			LadomOn();
		}
	}
}

void CWoodBoiler::Phase4()//Gesta
{
	if (watertemp >= ladomattemp && temp_status == TS_Growing && !ladom_on)
	{
		LadomOn();
		return;
	}

	if(watertemp >= ladomattemp + 5.0 && temp_status == TS_Growing)
	{
		EnterPhase3();//uzsikurem
		return;
	}

	if(watertemp > 30.0 && temp_status == TS_Growing)//Dadejom malku, Ikurimas
	{
		EnterPhase2();
		return;
	}

	if (watertemp < ladomattemp && ladom_on)
	{
		LadomOff();
	}

	if(watertemp < ladomattemp - 2.0 && temp_status == TS_Lowering)
	{
		EnterPhase1();//uzgesom
	}

	if(chimneytem_enabled)
	{
		if(chimneytemp <= watertemp)
		{
			EnterPhase1();//uzgesom
		}
	}
}

const char* CWoodBoiler::getTempStatusString(){
	switch (temp_status)
	{
	case TS_GrowingFast:
		return cTS_GrowingFast;
		break;
	case TS_Growing:
		return cTS_Growing;
		break;
	case TS_Stable:
		return cTS_Stable;
		break;
	case TS_Lowering:
		return cTS_Lowering;
		break;
	case TS_LoweringFast:
		return cTS_LoweringFast;
		break;
	default:
		return cTS_Default;
		break;
	}
}

void CWoodBoiler::PrintWoodBoilerStatus()
{
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
	root[jKeyTarget] = jTargetWoodBoiler;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	root[jKeyContent] = jTargetWoodBoiler;
	root[jWoodBoilerCurrentTemp] = watertemp;
	root[jKeyStatus] = Events::GetStatusName(status);
	root[jWoodBoilerTempStatus] = getTempStatusString();
	PrintJson.Print(root);
}

void CWoodBoiler::PrintLadomatStatus()
{
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
	root[jKeyTarget] = jTargetWoodBoiler;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	root[jKeyContent] = jWoodBoilerLadomatStatus;
	root[jKeyValue] = Events::GetStatusName(ladomat_status);
	PrintJson.Print(root);
}

void CWoodBoiler::PrintExhaustFanStatus()
{
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
	root[jKeyTarget] = jTargetWoodBoiler;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	root[jKeyContent] = jWoodBoilerSmokeFanStatus;
	root[jKeyValue] = Events::GetStatusName(exhaustfan_status);
	PrintJson.Print(root);
}

void CWoodBoiler::ParseJson(JsonObject jo)
{

}

void CWoodBoiler::ApplyConfig(uint8_t cfgPos, const WoodBoilerData &data)
{
	configPos = cfgPos;
	SetName(data.Name);
	SetWorkingTemp(data.WorkingTemp / 100.0f);
	SetHisteresis(data.WorkingHisteresis / 100.0f);
	SetLadomatChannel(data.LadomatChannel);
	SetExhaustFanChannel(data.ExhaustFanChannel);
	SetTempSensor(TSensors.getByName(data.TSensorName));
	SetKTypeSensor(KTypes.GetByName(data.KTypeName));
	SetLadomatTemp(data.LadomatTemp / 100.0f);
	SetLadomatTrigger(TempTriggers.GetByName(data.LadomatTempTriggerName));
	   	  
	char WaterBoilerName[MAX_NAME_LENGTH]; // <- nauja
}

/*********************************************
*********************************************/

CWoodBoiler* CWoodBoilers::GetByName(const char* name)
{
	CWoodBoiler* boiler;
	for (int i = 0; i<Count(); i++)
	{
		boiler = Get(i);
		if (strcmp(name, boiler->GetName()) == 0) return boiler;
	}
	return NULL;
}

void CWoodBoilers::Begin()
{
	for (int i = 0; i < Count(); i++)
	{
		CWoodBoiler* boiler = Get(i);
		if (boiler) boiler->Begin();
	}
}

void CWoodBoilers::Reset()
{
	for (int i = 0; i < Count(); i++)
	{
		CWoodBoiler* boiler = Get(i);
		if (boiler) boiler->Reset();
	}
}

void CWoodBoilers::LoadConfig(void)
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_WOOD_BOILERS_COUNT; i++)
		{
			WoodBoilerData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					CWoodBoiler* wdb = Add();
					if (wdb)
					{
						wdb->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[64];
				sprintf(s, "WoodBoiler config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_WOOD_BOILERS_COUNT; i++)
		{
			const WoodBoilerData& data = Defaults.GetDefaultWoodBoilerData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					CWoodBoiler* wdb = Add();
					if (wdb)
					{
						wdb->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				char s[64];
				sprintf(s, "WoodBoiler config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_WOOD_BOILERS_COUNT; i++)
		{
			const WoodBoilerData& data = Defaults.GetDefaultWoodBoilerData(i);
			if (data.Valid)
			{
				CWoodBoiler* wdb = Add();
				if (wdb)
				{
					wdb->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool CWoodBoilers::GetConfigData(uint8_t cfgPos, WoodBoilerData & data)
{
	if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
	{
		uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
		if (FileManager.FileSeek(fpos))
		{
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				FileManager.FileClose();
				return true;
			}
		}
		FileManager.FileClose();
	}
	return false;
}

bool CWoodBoilers::ValidateSetupDataSet(JsonObject & jo)
{
	return jo.containsKey(jKeyConfigPos) &&
		jo.containsKey(jKeyName) &&
		jo.containsKey(jWoodBoilerTempSensorName) &&
		jo.containsKey(jWoodBoilerKTypeName) &&
		jo.containsKey(jWoodBoilerLadomatChannel) &&
		jo.containsKey(jWoodBoilerExhaustFanChannel) &&
		jo.containsKey(jWoodBoilerWorkingTemp) &&
		jo.containsKey(jWoodBoilerHisteresis) &&
		jo.containsKey(jWoodBoilerLadomatWorkingTemp) &&
		jo.containsKey(jWoodBoilerLadomatTriggerName) &&
		jo.containsKey(jWoodBoilerWBName);
}

void CWoodBoilers::ParseJson(JsonObject& jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char * action = jo[jKeyAction];
		if (strcmp(action, jWoodBoilerActionIkurimas) == 0)
		{
			if (jo.containsKey(jKeyName))
			{
				CWoodBoiler* wb = GetByName(jo[jKeyName]);
				if (wb)
				{
					wb->EnterPhase2();
				}
			}
			else
			{
				PrintJson.PrintResultError(jTargetWoodBoiler, jErrorInvalidDataSet);
			}
		}
		if (strcmp(action, jWoodBoilerActionSwitch) == 0)
		{
			if (jo.containsKey(jKeyName))
			{
				CWoodBoiler* wb = GetByName(jo[jKeyName]);
				if (wb)
				{
					const char * target = jo[jKeyValue];
					if (strcmp(target, jWoodBoilerTargetExhaust) == 0)
					{
						wb->ChangeExhaustFanManual();
					}
					else if (strcmp(target, jWoodBoilerTargetLadomat) == 0)
					{
						wb->ChangeLadomatManual();
					}
				}
			}
			else
			{
				PrintJson.PrintResultError(jTargetWoodBoiler, jErrorInvalidDataSet);
			}
		}
		if (strcmp(action, jKeySetup) == 0)
		{
			if (ValidateSetupDataSet(jo))
			{
				WoodBoilerData data;
				uint8_t cfgPos = jo[jKeyConfigPos];
				Utilities::ClearAndCopyString(jo[jKeyName], data.Name);
				Utilities::ClearAndCopyString(jo[jWoodBoilerTempSensorName], data.TSensorName);
				Utilities::ClearAndCopyString(jo[jWoodBoilerKTypeName], data.KTypeName);
				data.LadomatChannel = jo[jWoodBoilerLadomatChannel];
				data.ExhaustFanChannel = jo[jWoodBoilerExhaustFanChannel];
				data.WorkingTemp = (int)((float)jo[jWoodBoilerWorkingTemp] * 100);
				data.WorkingHisteresis = (int)((float)jo[jWoodBoilerHisteresis] * 100);
				data.LadomatTemp = (int)((float)jo[jWoodBoilerLadomatWorkingTemp] * 100);
				Utilities::ClearAndCopyString(jo[jWoodBoilerLadomatTriggerName], data.LadomatTempTriggerName);
				Utilities::ClearAndCopyString(jo[jWoodBoilerWBName], data.WaterBoilerName);

				uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
				if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
				{
					if (FileManager.FileSeek(fpos))
					{
						if (FileManager.FileWriteBuff(&data, sizeof(data)))
						{
							FileManager.FileClose();
							SystemStatus.SetRebootRequired();
							PrintJson.PrintResultOk(jTargetWoodBoiler, jKeySetup, true);
							return;
						}
					}
					PrintJson.PrintResultError(jTargetWoodBoiler, jErrorFileWriteError);
					return;
				}
				PrintJson.PrintResultError(jTargetWoodBoiler, jErrorFileOpenError);
				return;
			}
		}
	}
	else
	{
		PrintJson.PrintResultFormatError();
	}
	
}

CWoodBoilers WoodBoilers;