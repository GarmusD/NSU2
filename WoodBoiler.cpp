#include "WoodBoiler.h"


//KatilasClass Katilas;

CWoodBoiler::CWoodBoiler(void)
{
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
	lastcheck = -30000;
	temp_array_check = 0;
	//relay = RelayModule::getInstance();
	working_temp = 75.0;
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
	temp_status = TS_Stable;
	temp_array_add = true;
	for(byte i=0; i < TEMP_ARRAY_SIZE; i++){
		temp_array[i] = -55.5;
	}
	StartingPhase();
	katilasInfoTime = 0;

	opmodeswitch = NULL;
	burnmodeswitch = NULL;
}

CWoodBoiler::~CWoodBoiler(void)
{
}

void CWoodBoiler::Begin()
{
	TimeSlice.RegisterTimeSlice(this);

	opmodeswitch = Switches.GetByName(SWITCH_NAMES[0].c_str());
	if (opmodeswitch != NULL)
	{
		opmodeswitch->AddStatusChangeHandler(this);
		HandleStatusChange(opmodeswitch, opmodeswitch->GetStatus());
	}
	burnmodeswitch = Switches.GetByName(SWITCH_NAMES[3].c_str());
	//burnmodeswitch->AddStatusChangeHandler(this);
}

bool CWoodBoiler::SetSimulationMode(bool value)
{
	if(value && watertemp >= working_temp + 2.0)
	{
		Log.debug("Simulation disabled because water_temp is too high: "+String(watertemp));
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
	Log.debug("Unknown simulation error.");
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
			Log.debug("KatilasClass::DoBoilerRoutine() - Nezinoma bukle!!! Status: " +Events::GetStatusName(status));
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
			}
			lastcheck = millis();
			DoBoilerRoutine();
		}
	}else if(Sender == chimneytempsens){
		chimneytemp = temp;
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
		for(byte i=0; i<TEMP_ARRAY_SIZE; i++){
			temp_array[i] = value;
		}
	}else{
		for(byte i=1; i < TEMP_ARRAY_SIZE; i++){
			temp_array[i-1] = temp_array[i];
		}
		temp_array[TEMP_ARRAY_SIZE-1] = value;
	}
	//calculate temp_status
	float t1 = 0;
	float t2 = 0;
	byte c1 = 0;
	byte c2 = 0;

	for(byte i=0; i < TEMP_ARRAY_SIZE / 2; i++){
		float t = temp_array[i];
			t1 += t;
			c1++;
	}

	for(byte i=TEMP_ARRAY_SIZE / 2; i < TEMP_ARRAY_SIZE; i++){
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
	if(watertempsens){
		Log.debug("Katilas TSensor set: "+String(sensor->getName()));
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
	if(chimneytempsens){
		Log.debug("Katilas KType Sensor set: "+String(sensor->GetName()));
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

void CWoodBoiler::SetLadomatChannel(byte idx)
{
	ladom_channel = idx;
	if(ladom_on && ladom_channel != 0xFF){		
		ladomat_status = STATUS_LADOMAT_ON;
		PrintLadomatStatus();
		DispatchStatusChange(this, ladomat_status);
		RelayModules.OpenChannel(ladom_channel);
	}
}

void CWoodBoiler::SetExhaustFanChannel(byte idx)
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
	working_temp = work_temp / 100.0;
}

void CWoodBoiler::SetHisteresis(float temp){
	histeresis = temp / 100.0;
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
		ChangeExhaustFanStatus(STATUS_EXHAUST_MANUAL);
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
	if(ladom_channel != 0xFF){		
		ChangeLadomatStatus(STATUS_LADOMAT_ON);
		RelayModules.OpenChannel(ladom_channel);		
	}
}

void CWoodBoiler::LadomOff()
{
	if(!ladom_on) return;
	ladom_on = false;
	if(ladom_channel != 0xFF && !ladom_manual){		
		ChangeLadomatStatus(STATUS_LADOMAT_OFF);
		RelayModules.CloseChannel(ladom_channel);
	}
}

void CWoodBoiler::VentOn()
{
	if(exhaust_on) return;
	exhaust_on = true;
	exhaust_start = millis();
	if(exhaustfan_channel != 0xFF){		
		ChangeExhaustFanStatus(STATUS_EXHAUSTFAN_ON);
		RelayModules.OpenChannel(exhaustfan_channel);
	}
}

void CWoodBoiler::VentOff()
{
	if(!exhaust_on) return;
	exhaust_on = false;
	if(exhaustfan_channel != 0xFF && !exhaust_manual){
		ChangeExhaustFanStatus(STATUS_EXHAUSTFAN_OFF);
		RelayModules.CloseChannel(exhaustfan_channel);
	}
}

void CWoodBoiler::ChangeExhaustFanStatus(Status newStatus)
{
	exhaustfan_status = newStatus;
	PrintExhaustFanStatus();
	Log.info("KATILAS: Status: " +Events::GetStatusName(exhaustfan_status));
	DispatchStatusChange(this, exhaustfan_status);
}

void CWoodBoiler::ChangeLadomatStatus(Status newStatus)
{
	ladomat_status = newStatus;
	PrintLadomatStatus();
	Log.info("KATILAS: Status: " +Events::GetStatusName(ladomat_status));
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
	//Jungiam ladomata ir ventiliatoriu
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

String CWoodBoiler::GetInfoString(){
	String info = "INFO: KATILAS: FullInfo: "+GetStatusName(status)+" " + String((int)(watertemp*100))+" "+String(ladom_on)+" "+String(ladom_manual)+" "+String(exhaust_on)+" "+String(exhaust_manual)+" "+String(temp_status);
	return info;
}

String CWoodBoiler::getTempStatusString(){
	switch (temp_status)
	{
	case TS_GrowingFast:
		return "GrowingFast";
		break;
	case TS_Growing:
		return "Growing";
		break;
	case TS_Stable:
		return "Stable";
		break;
	case TS_Lowering:
		return "Lowering";
		break;
	case TS_LoweringFast:
		return "LoweringFast";
		break;
	default:
		return "Unknown";
		break;
	}
}

void CWoodBoiler::PrintWoodBoilerStatus()
{
	StaticJsonBuffer<512> jBuff;
	JsonObject& root = jBuff.createObject();
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
	StaticJsonBuffer<512> jBuff;
	JsonObject& root = jBuff.createObject();
	root[jKeyTarget] = jTargetWoodBoiler;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	root[jKeyContent] = jWoodBoilerLadomatStatus;
	root[jKeyValue] = Events::GetStatusName(ladomat_status);
	PrintJson.Print(root);
}

void CWoodBoiler::PrintExhaustFanStatus()
{
	StaticJsonBuffer<512> jBuff;
	JsonObject& root = jBuff.createObject();
	root[jKeyTarget] = jTargetWoodBoiler;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	root[jKeyContent] = jWoodBoilerSmokeFanStatus;
	root[jKeyValue] = Events::GetStatusName(exhaustfan_status);
	PrintJson.Print(root);
}

void CWoodBoiler::ParseJson(JsonObject& jo)
{

}

byte CWoodBoiler::FillWoodBoilerData(WoodBoilerData &data)
{
	data.Valid = 1;
	strncpy(data.Name, name, MAX_NAME_LENGTH);
	strncpy(data.TSensorName, watertempsens->getName(), MAX_NAME_LENGTH);
	strncpy(data.KTypeName, chimneytempsens->GetName(), MAX_NAME_LENGTH);
	data.LadomatChannel = ladom_channel;
	data.ExhaustFanChannel = exhaustfan_channel;
	data.WorkingTemp = (int)(working_temp * 100);
	data.WorkingHisteresis = (int)(histeresis * 100);
	data.LadomatTemp = (int)(ladomattemp * 100); //<- nauja
	strncpy(data.LadomatTempTriggerName, ladomat_trigger->GetName(), MAX_NAME_LENGTH); // <- nauja
	return 0;//dbid
}

void CWoodBoiler::SetSettings(WoodBoilerData &data)
{
	SetName(data.Name);
	SetWorkingTemp(data.WorkingTemp);
	SetHisteresis(data.WorkingHisteresis);
	SetLadomatChannel(data.LadomatChannel);
	SetExhaustFanChannel(data.ExhaustFanChannel);
	SetTempSensor(TSensors.getByName(data.TSensorName));
	SetKTypeSensor(KTypes.GetByName(data.KTypeName));
	SetLadomatTemp(data.LadomatTemp / 100.0f);
	SetLadomatTrigger(TempTriggers.GetByName(data.LadomatTempTriggerName));
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

void CWoodBoilers::ParseJson(JsonObject& jo)
{
	if (jo.containsKey(jKeyName))
	{
		CWoodBoiler* wb = GetByName(jo[jKeyName]);
		if (wb)
		{
			if (jo.containsKey(jKeyAction))
			{
				String action = jo[jKeyAction];
				if (action.equals(jWoodBoilerActionIkurimas))
				{
					wb->EnterPhase2();
					return;
				}
				if (action.equals(jWoodBoilerActionSwitch))
				{
					String target = jo[jKeyValue];
					if (target.equals(jWoodBoilerTargetExhaust))
					{
						wb->ChangeExhaustFanManual();
					}
					else if (target.equals(jWoodBoilerTargetLadomat))
					{
						wb->ChangeLadomatManual();
					}
					return;
				}
				if (action.equals(jKeySetup))
				{
					WoodBoilerData data;
					byte pos = wb->FillWoodBoilerData(data);
					if (jo.containsKey(jKeyName))
					{
						strncpy(data.Name, jo[jKeyName], MAX_NAME_LENGTH);
					}
					if (jo.containsKey(jWoodBoilerTempSensorName))
					{
						strncpy(data.TSensorName, jo[jWoodBoilerTempSensorName], MAX_NAME_LENGTH);
					}
					if (jo.containsKey(jWoodBoilerKTypeName))
					{
						strncpy(data.KTypeName, jo[jWoodBoilerKTypeName], MAX_NAME_LENGTH);
					}
					if (jo.containsKey(jWoodBoilerLadomatChannel))
					{
						data.LadomatChannel = jo[jWoodBoilerLadomatChannel];
					}
					if (jo.containsKey(jWoodBoilerExhaustFanChannel))
					{
						data.ExhaustFanChannel = jo[jWoodBoilerExhaustFanChannel];
					}
					if (jo.containsKey(jWoodBoilerWorkingTemp))
					{
						data.WorkingTemp = (int)((float)jo[jWoodBoilerWorkingTemp] * 100);
					}
					if (jo.containsKey(jWoodBoilerHisteresis))
					{
						data.WorkingHisteresis = (int)((float)jo[jWoodBoilerHisteresis] * 100);
					}
					if (jo.containsKey(jWoodBoilerLadomatWorkingTemp))
					{
						data.LadomatTemp = (int)((float)jo[jWoodBoilerLadomatWorkingTemp] * 100);
					}
					if (jo.containsKey(jWoodBoilerLadomatTriggerName))
					{
						strncpy(data.LadomatTempTriggerName, jo[jWoodBoilerLadomatTriggerName], MAX_NAME_LENGTH);
					}
					Settings.setWoodBoilerData(pos, data);
					wb->SetSettings(data);
					PrintJson.PrintResultOk(jTargetWoodBoiler, jKeySetup);
				}
			}
		}
	}
}

CWoodBoilers WoodBoilers;