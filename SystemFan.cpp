#include "SystemFan.h"



SystemFan::SystemFan()
{
	ts = NULL;
	mint = 25;
	maxt = 35;
}


SystemFan::~SystemFan()
{
}

void SystemFan::Begin()
{
	pinMode(8, OUTPUT);
}

void SystemFan::SetTempSensor(TempSensor * sensor)
{
	if (ts)
	{
		ts->RemoveTempChangeHandler(this);
		ts = NULL;
	}
	if (sensor)
	{
		ts = sensor;
		ts->AddTempChangeHandler(this);
		HandleTemperatureChange(ts, ts->getTemp());
	}
}

void SystemFan::SetTemperatures(float min, float max)
{
	mint = min;
	maxt = max;
}

void SystemFan::HandleTemperatureChange(void * Sender, float value)
{
	byte val = 0;
	if (value >= mint)
	{
		val = map(value, mint, maxt, 125, 255);
	}
	analogWrite(8, val);
	Log.debug("SYSFAN: SysFan PWM value: " + String(val));
}

void SystemFan::OnTimeSlice()
{
}

void SystemFan::ParseJSON(JsonObject & jo)
{
}

SystemFan SysFan;