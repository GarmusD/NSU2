//"D:\arduino/hardware/tools/bossac.exe" -i -d --port=COM7 -U false -e -w -v -b "C:\Users\Dainius\AppData\Local\VMicro\Arduino\Builds\NSU2\arduino_due_x_dbg/NSU2.bin" -R
// Raspberry Pi MAC address: B8:27:EB:34:C8:DE
//System TSensor 28:77:6A:94:4:0:0:DE

#define USE_UI 1

#include <Base64.h>
#include <ArduinoJson.h>
#include <DueTimer.h>

#include <BButton.h>
#include <SdFat.h>

#include <myWdt.h>
#include <myTrng.h>
#include <myRtc.h>
#include <advancedFunctions.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "JsonPrinter.h"
#include "TimeSlice.h"
#include "Logger.h"
#include "TSensors.h"
#include "EEPROM.h"
#include "FreeMem.h"
#include "WoodBoiler.h"
#include "Settings.h"
#include "CmdServer.h"
#include "CmdMessenger.h"
#include "aJson.h"
#include "FileManager.h"
#include "SystemStatus.h"
#include "BootChecker.h" 
#include "SystemFan.h"
#include "consts.h"

#if USE_UI
#include <UTouchCD.h>
#include <UTouch.h>
#include <UTFT.h>
#include <memorysaver.h>
#include "windows.h"
#include "WindowSplash.h"
#include "WindowKatiline.h"
#include "SideWindow.h"
#include "graphics.h"

UTFT utft(CPLD, 25, 26, 27, 28);
UTouch utouch(6, 5, 32, 3, 2);

CWindows Windows(utft, utouch);

WindowKatiline* katiline;
Splash* splash;
#endif

uint32_t guard_time = 0;

#define BUTTON_PIN 20
BButton button = BButton(BUTTON_PIN);

void OnButtonRelease(BButton& btn) 
{
	Log.debug("Fizinis Ikurimo mygtukas paspaustas.");
	Windows.ForceLCDBacklight();
	WoodBoilers.Get(0)->EnterPhase2();
}

void SetInfoText(String tekstas)
{
#if USE_UI
	Log.debug("SetInfoText("+tekstas+")");
	if (splash)
	{
		splash->SetInfoText(tekstas);
	}
#endif
}

void UpdateInfoText(String tekstas)
{
#if USE_UI
	Log.debug("UpdateInfoText(" + tekstas + ")");
	if (splash)
	{
		splash->UpdateInfoText(tekstas);
	}
#endif
}

#if USE_UI

#include "ArduinoJson.h"
void PrintPinStatus()
{
	int pa = REG_PIOA_PDSR;
	int pb = REG_PIOB_PDSR;
	int pc = REG_PIOC_PDSR;
	int pd = REG_PIOD_PDSR;

	StaticJsonBuffer<2048> jBuff;
	JsonObject& root = jBuff.createObject();
	root[jKeyTarget] = jTargetSystem;
	root[jKeyAction] = "pinstate";
	
	for (int i = 1; i < NUM_DIGITAL_PINS; i++)
	{
		//Pio* p = digitalPinToPort(i);
		//int m = digitalPinToBitMask(i);
		//int v = p->PIO_PDSR;
		//root[String(i)] = !!(v & m) ? 1 : 0;
		root[String(i)] = digitalRead(i);
	}

	PrintJson.Print(root);
}
#endif

void setup()
{
	pinMode(MAX31855_CS, INPUT);
	pinMode(MAX31855_CS, OUTPUT);  //CS is an Output 
	digitalWrite(MAX31855_CS, HIGH);
	RelayModules.Init();
	FileManager.Init();

	SPI.begin();
	SPI.setClockDivider(128);

	//delay(100);

	Serial.begin(115200);
	delay(500);
	Log.begin();

	//////////////////////////////////////////////////////////////////////////////////////
	//Iskelti i funkcija

	SystemStatus.SetState(SystemBooting);
	Log.debug("Starting...");
	Log.log("NSU_Starting");

#if USE_UI
	Log.debug("Initializing Windows.Begin(&utft, &utouch);");
	//Windows = new CWindows();
	Windows.Begin();
	Log.debug("Free mem after windows initialized: " + String(FreeMem::GetFreeMem()));
#endif

	Log.debug("Setting up FileManager.");
	FileManager.Begin();
	if (FileManager.SDCardOk())
	{
		Log.debug("SDCard initialized OK.");
	}
	else
	{
		Log.debug("SDCard initialization failed.");
	}
	Log.debug("Executing BootChecker.CheckBootWait()");
	BootChecker.CheckBootWait(3);

	Log.debug("Creating EEPROM Emulation class.");
	EEPROM.Begin();

#if USE_UI
	Log.debug("Creating splash window...");
	splash = new Splash(Windows.getAvailableArea(), Windows.getUTFT(), Windows.getScenario(), NULL);
	Log.debug("Free mem after splash created: " + String(FreeMem::GetFreeMem()));

	Windows.AddWindow(splash);
	Log.debug("Free mem after windows->AddWindow(): " + String(FreeMem::GetFreeMem()));
	splash->Draw();
	Log.debug("Free mem after splash->Draw(): " + String(FreeMem::GetFreeMem()));
#endif
	//delay(2000);

	Log.debug("Collecting TSensors->...");
	TSensors.Begin();
	TSensors.SetResolution(12);
	SetInfoText("Rasta temperaturos davikliu: " + String(TSensors.getSensorCount()));
	while (TSensors.ScanSensors())
	{
		Log.debug("DBG: Sensors found: " + String(TSensors.getSensorCount()));
		UpdateInfoText("Rasta temperaturos davikliu: " + String(TSensors.getSensorCount()));
	}
	SetInfoText("Rasta temperaturos davikliu: " + String(TSensors.getSensorCount()) + ". Gaunamos temperaturos...");
	Log.debug("Reading initial temperatures...");
	TSensors.InitialReadAllTemperatures();//first temp after power up will be 85C
	//TSensors->ResetValid();

	//Settings
	Log.debug("Initializing Settings class.");
	Settings.Begin();
	SetInfoText("Nuskaitomi nustatymai...");
	Log.debug("Going to read settings.");
	if (!Settings.IsReady())
	{
		Log.error("Atminties kortele nepasiekiama.");
		SetInfoText("Atminties kortele nepasiekiama. Nustatymai pagal nutylejima.");
		delay(1000);
	}
	else
	{
		Settings.BeginLongRead();
	}

	//Switches
	Log.debug("Reading Switches.");
	for (byte i = 0; i < MAX_SWITCHES_COUNT; i++) 
	{
		Log.debug("    Reading Switch data: " + String(i));
		SwitchData data = Settings.getSwitchData(i);
		if (data.Valid) 
		{
			Log.debug("        Switch is Enabled.");
			Switch* swth = Switches.Add();
			if (swth)
			{
				swth.ApplySettings(i, data);
			}
			Log.debug("    Switch '" + String(data.Name) + "' loaded.");
		}
	}
	Log.debug("Switches readed. Free mem: " + String(FreeMem::GetFreeMem()));


	//TempSensors
	Log.debug("Reading TempSensors.");
	for (byte i = 0; i < MAX_SENSOR_COUNT; i++) 
	{
		Log.debug("    Reading TempSensor data: " + String(i));
		TSensorsData data = Settings.getTempSensorData(i);
		if (data.Valid) 
		{
			Log.debug("        TempSensor Enabled.");
			TempSensor* ts = TSensors.getByAddr(data.sid.addr);
			if (ts) 
			{
				Log.debug("        Setting TempSensor name: " + String(data.sid.name));
				ts->setName(data.sid.name);
				Log.debug("        Setting TempSensor interval: " + String(data.interval));
				ts->SetInterval(data.interval);
				ts->SetValid(true);
			}
			else
			{
				Log.error("    TempSensor NOT FOUND: " + TempSensor::GetSensorNameAddr(&data.sid));
			}
		}
	}
	Log.debug("TempSensors readed. Free mem: " + String(FreeMem::GetFreeMem()));


	//RelayModule
	/*********************************************/
	Log.debug("Reading RelayModules.");
	/*********************************************/
	for (byte i = 0; i < MAX_RELAY_MODULES; i++) 
	{
		RelayModuleData data = Settings.getRelayModuleData(i);
		if (data.Valid) 
		{
			//set relay
			Log.debug("        Setting relay module: ActiveLow: " + String(data.ActiveLow) + ", Inverted: " + String(data.Inverted));
			RelayModules.AddRelayModule(data.ActiveLow, data.Inverted);
		}
	}
	RelayModules.AllChannelsOff();
	RelayModules.Begin();
	Log.debug("RelayModules readed. Free mem: " + String(FreeMem::GetFreeMem()));


	//TempTriggers
	Log.debug("Reading TempTriggers.");
	for (byte i = 0; i < MAX_TEMP_TRIGGERS; i++) 
	{
		Log.debug("    Reading TempTrigger data: " + String(i));
		TempTriggerData data = Settings.getTempTriggerData(i);
		if (data.Valid) 
		{
			TempTrigger* temptrigger = TempTriggers.Add();
			if (temptrigger)
			{
				Log.debug("        Setting trigger name to: " + String(data.Name));
				temptrigger->SetName(data.Name);
				Log.debug("        Adding TempTrigger pieces");
				for (int i = 0; i < MAX_TEMP_TRIGGER_PIECES; i++)
				{
					if (data.ttpiece[i].Valid)
					{
						Log.debug("            Adding TempTrigger piece: ");
						Log.debug("                Condition: " + String(data.ttpiece[i].Condition));
						Log.debug("                SensorName: " + String(data.ttpiece[i].SensorName));
						Log.debug("                Temperature: " + String(data.ttpiece[i].Temperature / 100.0));
						Log.debug("                Histeresis: " + String(data.ttpiece[i].Histeresis / 100.0));
						temptrigger->AddTriggerPiece((TriggerCondition)data.ttpiece[i].Condition, data.ttpiece[i].SensorName, data.ttpiece[i].Temperature / 100.0, data.ttpiece[i].Histeresis / 100.0);
					}
				}
				temptrigger->PiecesDone();				
			}
			else
			{
				Log.error("Failed TempTriggers.Add().");
			}
			Log.debug("TempTrigger '" + String(data.Name) + "' loaded.");
		}
	}
	Log.debug("TempTriggers readed. Free mem: " + String(FreeMem::GetFreeMem()));


	//CircPumps
	Log.debug("Reading CircPumps.");
	for (byte i = 0; i < MAX_CIRCULATION_PUMPS; i++) 
	{
		Log.debug("    Reading CircPump data: " + String(i));
		CircPumpData data = Settings.getCircPumpData(i);
		if (data.Valid) 
		{
			CirculationPump* cpump = CirculationPumps.Add();
			if (cpump)
			{
				Log.debug("        Setting name to: " + String(data.Name));
				cpump->SetName(data.Name);
				Log.debug("        Setting relay channel: " + String(data.Channel));
				cpump->SetRelayChannel(data.Channel);
				Log.debug("        Setting MaxSpeed, Speed1, Speed2, Speed3: " + String(data.MaxSpeed) + ", "+String(data.Spd1Channel)+", " + String(data.Spd2Channel) + ", " + String(data.Spd3Channel));
				cpump->SetMaxSpeed(data.MaxSpeed, data.Spd1Channel, data.Spd2Channel, data.Spd3Channel);
				if (strlen(data.TempTriggerName)) 
				{
					Log.debug("        Circulation pump " + String(i) + " SetTempTrigger: " + String(data.TempTriggerName));
					cpump->SetTempTrigger(TempTriggers.GetByName(data.TempTriggerName));
				}
				Log.debug("Circulation pump " + String(i) + " load done.");
			}
			else
			{
				Log.error("Failed CircPumps.Add().");
			}
		}
	}
	Log.debug("CircPumps readed. Free mem: " + String(FreeMem::GetFreeMem()));


	//Rankiniu budu randam boilerio cirkuliacini ir padarom ji standalone
	//CirculationPump* cpump = cpumps->GetByName("boilerio");
	//if(cpump){
	//	cpump->SetStandalone(true);
	//}

	//CollectorsData
	Log.debug("Reading Collectors.");
	for (byte i = 0; i < MAX_COLLECTORS; i++) 
	{
		Log.debug("    Reading Collectors data: " + String(i));
		CollectorsData data = Settings.getCollectorsData(i);
		if (data.Valid) 
		{
			Collector* coll = Collectors.Add();
			if (coll)
			{
				Log.debug("        Setting name: " + String(data.Name));
				coll->SetName(data.Name);
				Log.debug("        Setting valve count: " + String(data.valve_count));
				coll->SetValveCount(data.valve_count);
				for (byte j = 0; j < data.valve_count; j++) 
				{
					Log.debug("            Setting valve "+String(j)+": Ch: " + String(data.valves[j].relay_channel)+", Type: "+String(data.valves[j].type));
					coll->SetThermoValve(j, data.valves[j]);
				}
				Log.debug("        Setting circpump: " + String(data.CircPump));
				coll->SetCirculationPump(CirculationPumps.GetByName(data.CircPump));
			}
			else
			{
				Log.error("Failed Collectors.Add().");
			}
			Log.debug("!!!!! ValveCountTest at setup() end !!!!!");
			for (byte x = 0; x < Collectors.Count(); x++)
			{
				Collector* coll = Collectors.Get(x);
				Log.debug("!!!!! !!!!!! Collector " + String(coll->GetName()) + " valve count: " + String(coll->GetValveCount()));
			}
			Log.debug("!!!!! ValveCountTest DONE !!!!!");
		}
	}
	Log.debug("Collectors readed. Free mem: " + String(FreeMem::GetFreeMem()));


	//ComfortZonesData
	Log.debug("Reading ComfortZones.");
	for (byte i = 0; i < MAX_COMFORT_ZONES; i++) 
	{
		Log.debug("    Reading ComfortZones data: " + String(i));
		ComfortZonesData data = Settings.getComfortZonesData(i);
		if (data.Valid) 
		{
			ComfortZone* czone = ComfortZones.Add();
			if (czone)
			{
				Log.debug("        Getting title: " + String(czone->GetTitle()));
				Log.debug("        Setting name: " + String(data.Name));
				czone->SetName(data.Name);
				Log.debug("        Setting title: " + String(data.Title));
				czone->SetTitle(data.Title);
				Log.debug("        Setting collector: " + String(data.col_name)+" on channel "+String(data.channel));
				czone->SetCollector(Collectors.GetByName(data.col_name), data.channel);
				Log.debug("        Setting temperatures: " + String(data.Name));
				Log.debug("            RH: "+String((float)data.room_temp_hi / 100.0f)+", RL: " + String((float)data.room_temp_low / 100.0f) + ", FH: " + String((float)data.floor_temp_hi / 100.0f) + ", FL: " + String((float)data.floor_temp_low / 100.0f) + ", H: " + String((float)data.histeresis / 100.0f));
				czone->SetRequestedTemperature(
					(float)data.room_temp_hi / 100.0f,
					(float)data.room_temp_low / 100.0f,
					(float)data.floor_temp_hi / 100.0f,
					(float)data.floor_temp_low / 100.0f,
					(float)data.histeresis / 100.0f
				);
				if (strlen(data.room_sensor))
				{
					Log.debug("        Setting room sensor: " + String(data.room_sensor));
					czone->SetRoomSensor(TSensors.getByName(data.room_sensor));
				}
				if (strlen(data.floor_sensor))
				{
					Log.debug("        Setting floor sensor: " + String(data.floor_sensor));
					czone->SetFloorSensor(TSensors.getByName(data.floor_sensor));
				}
				Log.debug("        Setting room sensor enabled: " + String(true));
				czone->SetRoomSensorEnabled(true);//EEPROM.GetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[i]));
				//czone->SetFloorSensorEnabled(EEPROM.GetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[i]));
				//czone->SetIndex(i);
				Log.debug("        Starting comfort zone (begin()).");
				czone->Begin();
			}
			else
			{
				Log.error("Failed ComfortZones.Add().");
			}
		}
	}
	Log.debug("ComfortZones readed. Free mem: " + String(FreeMem::GetFreeMem()));


	//KTypeData
	Log.debug("Reading KTypes.");
	for (byte i = 0; i < MAX_KTYPE_SENSORS; i++)
	{
		Log.debug("    Reading KType data: " + String(i));
		KTypeData data = Settings.getKTypeData(i);
		if (data.Valid)
		{
			MAX31855* ktype = KTypes.Add();
			if (ktype)
			{
				Log.debug("        Setting name: " + String(data.Name));
				ktype->SetName(data.Name);
				Log.debug("        Setting interval: " + String(data.interval));
				ktype->SetTempReadInterval(data.interval);
				Log.debug("        Calling begin()");
				ktype->Begin();
			}
			else
			{
				Log.error("Failed KTypes.Add().");
			}
		}
	}
	Log.debug("KTypeSensors readed. Free mem: " + String(FreeMem::GetFreeMem()));


	//WaterBoilerData
	Log.debug("Reading WaterBoilers.");
	{
		Log.debug("    Reading WaterBoiler data: 0");
		WaterBoilerData data = Settings.getWaterBoilerData(0);
		if (data.Valid)
		{
			CWaterBoiler* WaterBoiler = WaterBoilers.Add();
			if (WaterBoiler)
			{
				Log.debug("        Setting name: " + String(data.Name));
				WaterBoiler->SetName(data.Name);
				Log.debug("        Setting circpump: " + String(data.CircPumpName));
				WaterBoiler->SetCircPump(CirculationPumps.GetByName(data.CircPumpName));
				Log.debug("        Setting temptrigger: " + String(data.TempTriggerName));
				WaterBoiler->SetTempTrigger(TempTriggers.GetByName(data.TempTriggerName));
				Log.debug("        Setting ElPowerChannel: " + String(data.ElHeatingChannel));
				WaterBoiler->SetElPowerChannel(data.ElHeatingChannel);
				Log.debug("        Setting ElPOwerData: " + String(data.Name));
				for (byte i = 0; i < 7; i++)
				{
					Log.debug("            Enabled: " + String(data.HeatingData[i].Enabled)+", StartHour:"+String(data.HeatingData[i].StartHour) + ", StartMin:" + String(data.HeatingData[i].StartMin) + ", EndHour:" + String(data.HeatingData[i].EndHour) + ", EndMin:" + String(data.HeatingData[i].EndMin));
					WaterBoiler->SetElPowerData(i, data.HeatingData[i]);
				}
				Log.debug("        WaterBoiler->Begin()");
				WaterBoiler->Begin();
			}
			else
			{
				Log.error("Failed WaterBoilers.Add().");
			}
		}
		Log.debug("WaterBoiler readed. Free mem: " + String(FreeMem::GetFreeMem()));
	}


	//Katilas
	Log.debug("Reading WoodBoilers.");
	WoodBoilerData data = Settings.getWoodBoilerData(0);
	if (data.Valid)
	{
		Log.debug("    Reading WoodBoiler data: 0");
		CWoodBoiler* Katilas = WoodBoilers.Add();
		if (Katilas)
		{
			Log.debug("        Setting name: " + String(data.Name));
			Katilas->SetName(data.Name);
			Log.debug("        Setting WorkingTemp: " + String(data.WorkingTemp));
			Katilas->SetWorkingTemp(data.WorkingTemp);
			Log.debug("        Setting WorkingHisteresis: " + String(data.WorkingHisteresis));
			Katilas->SetHisteresis(data.WorkingHisteresis);
			Log.debug("        Setting LadomatChannel: " + String(data.LadomatChannel));
			Katilas->SetLadomatChannel(data.LadomatChannel);
			Log.debug("        Setting ExhaustFanChannel: " + String(data.ExhaustFanChannel));
			Katilas->SetExhaustFanChannel(data.ExhaustFanChannel);
			Log.debug("        Setting TSensorName: " + String(data.TSensorName));
			Katilas->SetTempSensor(TSensors.getByName(data.TSensorName));
			Log.debug("        Setting KTypeName: " + String(data.KTypeName));
			Katilas->SetKTypeSensor(KTypes.GetByName(data.KTypeName));
			Log.debug("        Setting LadomatTemp: " + String(data.LadomatTemp / 100.0f));
			Katilas->SetLadomatTemp(data.LadomatTemp / 100.0f);
			Log.debug("        Setting LadomatTempTriggerName: " + String(data.LadomatTempTriggerName));
			Katilas->SetLadomatTrigger(TempTriggers.GetByName(data.LadomatTempTriggerName));
			Log.debug("        Katilas->Begin()");
			Katilas->Begin();
		}
		else
		{
			Log.error("Failed WoodBoilers.Add().");
		}
	}
	Log.debug("Katilas readed. Free mem: " + String(FreeMem::GetFreeMem()));

	SysFan.Begin();
	SysFan.SetTempSensor(TSensors.getByName("arduino"));
	

	Log.debug("Settings.EndLongRead();");
	Settings.EndLongRead();

#if USE_UI
	Log.debug("Reading pin state...");
	PrintPinStatus();
	if (splash)
	{
		//Windows.CloseWindow(splash);
		splash = NULL;
	}
	Log.debug("Splash closed. Free mem: " + String(FreeMem::GetFreeMem()));

	SideWindow* sw = new SideWindow(Windows.getSideWindowArea(), Windows.getUTFT(), Windows.getScenario(), NULL);
	Windows.AddSideWindow(sw);

	Log.debug("Creating UISwitchButton* btnOpMode...");
	UISwitchButton* btnOpMode = new UISwitchButton(sw);
	btnOpMode->SetOnBMPBytes(snaige_bmp);
	btnOpMode->SetOffBMPBytes(saule_bmp);
	btnOpMode->SetSwitch(Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_WINTER_MODE].c_str()));

	Log.debug("Creating UISwitchButton* btnTempMode...");
	UISwitchButton* btnTempMode = new UISwitchButton(sw);
	btnTempMode->SetOnBMPBytes(temp_up_bmp);
	btnTempMode->SetOffBMPBytes(temp_down_bmp);
	btnTempMode->SetSwitch(Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_TEMPERATURE_MODE].c_str()));

	Log.debug("Creating UISwitchButton* btnKarstasVanduo...");
	UISwitchButton* btnKarstasVanduo = new UISwitchButton(sw);
	btnKarstasVanduo->SetOnBMPBytes(karstas_vanduo_on_bmp);
	btnKarstasVanduo->SetOffBMPBytes(karstas_vanduo_off_bmp);
	btnKarstasVanduo->SetSwitch(Switches.GetByName(SWITCH_NAMES[SWITCH_NAME_HOT_WATER_MODE].c_str()));

	Log.debug("Adding UISwitchButtons to SideWindows...");
	sw->AddButton(btnOpMode);
	sw->AddButton(btnTempMode);
	sw->AddButton(btnKarstasVanduo);

	Log.debug("Creating temp window...");
	CWindow* tempw = new CWindow(Windows.getAvailableArea(), Windows.getUTFT(), Windows.getScenario(), NULL);
	Log.debug("Creating TempLabel....");
	UITempLabel* tl = new UITempLabel(tempw);
	Log.debug("Valio!!! Pavyko!!!");

	Log.debug("Creating WindowKatiline...");
	katiline = new WindowKatiline(Windows.getAvailableArea(), Windows.getUTFT(), Windows.getScenario(), (byte*)g_katiline_bg);
	Log.debug("Windows->AddWindow(katiline);...");
	Windows.AddWindow(katiline);
	Log.debug("katiline->Draw();...");
	katiline->Draw();
	Log.debug("Katiline DONE...");
#endif //  USE_UI

	//Start command server for messages
	cmdServer.begin();

	wdt.enable(4000); //4000 ms of survival

	button.clickHandler(OnButtonRelease);

	SystemStatus.begin();//= set to running state and inform watcher

	guard_time = millis();
	Log.log("GUARD");

	Timers.Begin();
}

void loop()
{
	TimeSlice.TimeSlice();

	button.isPressed();

	wdt.restart();

	if (Utilities::CheckMillis(guard_time, 5000)) 
	{
		Log.log("GUARD");
	}
}
