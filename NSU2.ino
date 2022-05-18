//bossac.exe -i -d --port=COM7 -U false -e -w -v -b NSU2.bin -R
//bossac.exe -i -d --port=COM4 -U false -e -w -b NSU2.BIN -R <- NO -v parameter = NO VERIFY
// Raspberry Pi MAC address: B8:27:EB:34:C8:DE
//System TSensor 28:77:6A:94:4:0:0:DE

#define USE_UI 1
#define RELAY_TEST 0

#include <BButton.h>

//#include <base64.hpp>
#include <DueTimer.h>


#include <SdFat.h>

#include <myWdt.h>
#include <myTrng.h>
#include <myRtc.h>
#include <advancedFunctions.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "ArduinoJson.h"
#include "JsonPrinter.h"
#include "TimeSlice.h"
#include "Logger.h"
#include "TSensors.h"
#include "Collectors.h"
#include "ComfortZones.h"
#include "FreeMem.h"
#include "WoodBoilers.h"
#include "Defaults.h"
#include "CmdServer.h"
#include "CmdMessenger.h"
#include "Alarms.h"
#include "aJson.h"
#include "FileManager.h"
#include "SystemStatus.h"
#include "BootChecker.h" 
#include "SystemFans.h"
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
#endif


uint32_t guard_time = 0;

#define BUTTON_PIN 20
BButton button = BButton(BUTTON_PIN);

void OnButtonRelease(BButton& btn) 
{
	Log.debug("Fizinis Ikurimo mygtukas paspaustas.");
#if USE_UI
	Windows.ForceLCDBacklight();
#endif
	WoodBoilers.Get(0)->EnterPhase2();
}

void SetInfoText(const char* tekstas)
{
#if USE_UI
	char s[256];
	sprintf(s, "SetInfoText(%s)", tekstas);
	Log.debug(s);
	//if (splash)
	//{
	Windows.GetSplashScreen().SetInfoText(tekstas);
	//}
#endif
}

void UpdateInfoText(const char* tekstas)
{
#if USE_UI
	char s[256];
	sprintf(s, "UpdateInfoText(%s)", tekstas);
	Log.debug(s);
	//if (splash)
	//{
	Windows.GetSplashScreen().UpdateInfoText(tekstas);
	//}
#endif
}

void StartSerialCommunications()
{
	//Enable serial communications
	Serial.begin(115200);
	delay(200);

	//Enable logging
	Log.begin();
	Log.debug("Serial communications enabled.");
}

void Reset()
{
	Log.debug("Reset() called...");
	Log.debug("Timers.Reset();");
	Timers.Reset();
	Log.debug("TimeSlice.Reset();");
	TimeSlice.Reset();
	Log.debug("SystemStatus.Reset();");
	SystemStatus.Reset();
	Log.debug("BootChecker.Reset();");
	BootChecker.Reset();
	Log.debug("cmdServer.Reset();");
	cmdServer.Reset();
#if USE_UI
	Log.debug("Windows.Reset();");
	Windows.Reset();
#endif
	Log.debug("button.clickHandler(NULL);");
	button.clickHandler(NULL);
	Log.debug("Switches.Reset();");
	Switches.Reset();
	Log.debug("TSensors.Reset();");
	TSensors.Reset();
	Log.debug("RelayModules.Reset();");
	RelayModules.Reset();
	Log.debug("TempTriggers.Reset();");
	TempTriggers.Reset();
	Log.debug("CirculationPumps.Reset();");
	CirculationPumps.Reset();
	Log.debug("Collectors.Reset();");
	Collectors.Reset();
	Log.debug("ComfortZones.Reset();");
	ComfortZones.Reset();
	Log.debug("KTypes.Reset();");
	KTypes.Reset();
	Log.debug("WaterBoilers.Reset();");
	WaterBoilers.Reset();
	Log.debug("WoodBoilers.Reset();");
	WoodBoilers.Reset();
	Log.debug("Alarm.Reset();");
	Alarms.Reset();
	Log.debug("SysFans.Reset();");
	SysFans.Reset();
}

void PreBegin()
{

	Log.debug("PreBegin() called...");

#if USE_UI
	Log.debug("Initializing Windows.Begin();");
	Windows.Begin();
	Windows.BeginSplashWindow();
	Log.debug("Drawing splash window...");
	Windows.DrawSplashScreen();
	SetInfoText("Paleidziama...");
#endif

	//Start command server for messages
	cmdServer.begin();

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


	TSensors.Begin();
	TSensors.SetResolution(12);
}

void PostBegin()
{	
	Log.debug("PostBegin() called...");	

	Switches.Begin();
	KTypes.Begin();
	WaterBoilers.Begin();
	WoodBoilers.Begin();
	SysFans.Begin();
	button.clickHandler(OnButtonRelease);
	RelayModules.Begin();

	guard_time = millis();
	Log.log("GUARD");
	SystemStatus.begin();//= set to running state and inform watcher
	Timers.Begin();

#if USE_UI
	Log.debug("Disabling SplashWindow...");
	Windows.GetSplashScreen().DisableWindow();
	Log.debug("Windows. ... .Begin() calling...");
	Windows.BeginMainWindow();
	Windows.BeginSideWindow();
#endif

	Log.debug("PostBegin() DONE.");
}

void ReadSettings()
{
	char s[256];
	Log.debug("ReadSettings called...");	

	Log.debug("Collecting TSensors->...");
	SetInfoText("Ieskomi temperaturu sensoriai...");
	sprintf(s, "Rasta temperaturos davikliu: %d", TSensors.getSensorCount());
	SetInfoText(s);
	while (TSensors.ScanSensors())
	{
		sprintf(s, "Sensors found: %d", TSensors.getSensorCount());
		Log.debug(s);
		sprintf(s, "Rasta temperaturos davikliu: %d", TSensors.getSensorCount());
		UpdateInfoText(s);
	}
	sprintf(s, "Nuskaitomos temperaturos...", TSensors.getSensorCount());
	SetInfoText(s);
	Log.debug("Reading initial temperatures...");
	TSensors.InitialReadAllTemperatures();//first temp after power up will be 85C

	SetInfoText("Nuskaitomi nustatymai...");

	//Switches
	Log.debug("Reading Switches.");
	Switches.LoadConfig();
	sprintf(s, "Switches readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//TempSensors
	Log.debug("Reading TempSensors.");
	TSensors.LoadConfig();
	sprintf(s, "TempSensors readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//RelayModule
	Log.debug("Reading RelayModules.");
	RelayModules.LoadConfig();
	sprintf(s, "RelayModules readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//Alarm
	Log.debug("Reading Alarm.");
	Alarms.LoadConfig();
	sprintf(s, "Alarm readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//TempTriggers
	Log.debug("Reading TempTriggers.");
	TempTriggers.LoadConfig();
	sprintf(s, "TempTriggers readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//CircPumps
	Log.debug("Reading CircPumps.");
	CirculationPumps.LoadConfig();
	sprintf(s, "CircPumps readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//CollectorsData
	Log.debug("Reading Collectors.");
	Collectors.LoadConfig();
	sprintf(s, "Collectors readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//ComfortZonesData
	Log.debug("Reading ComfortZones.");
	ComfortZones.LoadConfig();
	sprintf(s, "ComfortZones readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//KTypeData
	Log.debug("Reading KTypes.");
	KTypes.LoadConfig();
	sprintf(s, "KTypeSensors readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//WaterBoilerData
	Log.debug("Reading WaterBoilers.");
	WaterBoilers.LoadConfig();
	sprintf(s, "WaterBoiler readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//Katilas
	Log.debug("Reading WoodBoilers.");
	WoodBoilers.LoadConfig();
	sprintf(s, "Katilas readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);

	//SystemFan
	Log.debug("Reading SystemFans.");
	SysFans.LoadConfig();
	sprintf(s, "SystemFans readed. Free mem: %d", FreeMem::GetFreeMem());
	Log.debug(s);
}

void ShowUI()
{
#if USE_UI
	Log.debug("Windows.DrawMainWindow()...");	
	Windows.DrawMainWindow();
	Log.debug("Windows.DrawMainWindow() DONE...");
	Log.debug("Windows.DrawSideWindow()...");
	Windows.DrawSideWindow();
	Log.debug("Windows.DrawSideWindow()...");
#endif //  USE_UI
}

void setup()
{
	//HW Initialization

	StartSerialCommunications();
	Log.debug("Initializing Hardware...");
	//Start SPI
	SPI.begin();
	//SPI.setClockDivider(128);???
	//delay(100);
	Log.debug("SPI enabled...");

	//Init other hardware before TFT
	Log.debug("Initializing KTypes HW...");
	KTypes.InitHW();

	Log.debug("Initializing RelayModules HW...");
	RelayModules.InitHW();

	Log.debug("Initializing FileManager HW...");
	FileManager.InitHW();

#if USE_UI
	//Init TFT
	Log.debug("Initializing Windows HW...");
	Windows.InitHW();
#endif

	//Enable watchdog
	Log.debug("Enabling watchdog...");
	watchdogEnable(5000);
	Log.debug("setup() DONE...");
}

void loop()
{
	Reset();

	//Inform booting state
	SystemStatus.SetState(SystemBooting);
	Log.debug("Starting...");
	Log.log("NSU_Starting");

	PreBegin();
	ReadSettings();
	PostBegin();
	ShowUI();

	bool led = false;
	uint32_t m = 0;
	pinMode(LED_BUILTIN, OUTPUT);

	Log.debug("Entering main loop...");
	Log.memInfo();

#if RELAY_TEST
	//RelayModules.AllChannelsOff();
	while (true)
	{
		for (byte i = 1; i < 25; i++)
		{
			RelayModules.OpenChannel(i);
			RelayModules.WriteChannels();
			delay(1000);
			RelayModules.CloseChannel(i);
			RelayModules.WriteChannels();
			delay(100);
		}
	}
#endif

	while (!SystemStatus.GetRebootReqested())
	{
		if (millis() - m >= 500)
		{
			m = millis();
			led = !led;
			
			if (led)
			{
				//Log.log("Writing led HIGH");
				digitalWrite(LED_BUILTIN, HIGH);
			}
			else
			{
				//Log.log("Writing led LOW");
				digitalWrite(LED_BUILTIN, LOW);
			}
		}
		TimeSlice.TimeSlice();

		button.isPressed();

		//wdt.restart();
		watchdogReset();

		if (Utilities::CheckMillis(guard_time, 5000))
		{
			Log.log("GUARD");
		}
	}
	Log.debug("Soft reset requested");
	Log.memInfo();
	//Perform "reboot"
#if USE_UI
	//Light up screen
	Windows.ForceLCDBacklight();
#endif
}
