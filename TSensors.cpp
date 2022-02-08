#include <math.h>
#include "TSensors.h"
#include <OneWire.h>
#include <DallasTemperature.h>

/*********************************************
*********************************************/
/*********************************************
*********************************************/

TempSensor::TempSensor()
{
	status = TR_DONE;
	enabled = false;
	currtemp = -127.0;
	repeat_count = 0;
	readcount = 0;
	interval = readinterval = 180000;//3 min
	lastread = INT32_MIN;
	lastdispatch = INT32_MIN;
	sid.name[0] = 0;
	for (int i = 0; i < 8; i++)
	{
		sid.addr[i] = 0;
	}
}

void TempSensor::SetAddress(const uint8_t* addr)
{
	for (int i = 0; i < 8; i++)
	{
		//saddr += String(addr[i], HEX); if (i < 7) saddr += ", ";
		sid.addr[i] = addr[i];
	}
	char saddr[MAX_SENSOR_NAME];
	sprintf(saddr, "%02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
	Log.debug("Added sensors addr: " + String(saddr));
}

void TempSensor::SetDallas(DallasTemperature* dtemp)
{
	dt = dtemp;
}

void TempSensor::SetValid(bool value)
{
	enabled = value;
}

bool TempSensor::GetValid()
{
	return enabled;
}

void TempSensor::SetInterval(uint32_t t)
{
	interval = t * 1000;
	readinterval = round(interval / average.GetSize());
}

uint32_t TempSensor::GetInterval() {
	return interval / 1000;
}

//String TempSensor::getName()
//{
//	return String(sid.name);
//}

const char* TempSensor::getName()
{
	return sid.name;
}

float TempSensor::getTemp()
{
	return currtemp;
}

float TempSensor::getAverageTemp()
{
	return avgtemp;
}

uint8_t* TempSensor::getAddr()
{
	return sid.addr;
}

void TempSensor::setName(const char* name)
{
	strncpy(sid.name, name, MAX_SENSOR_NAME);
}

float TempSensor::pround(float x)
{
	if (x == 0.)
		return x;
	bool sign = x < 0.;
	if (sign) x = abs(x);
	double dintpart;
	float fractpart, intpart;
	fractpart = modf(x, &dintpart);
	intpart = dintpart;
	if (fractpart < 0.25)
	{
		fractpart = 0.00001;
	}
	else if (fractpart < 0.75)
	{
		fractpart = 0.50001;
	}
	else
	{
		fractpart = 1.00001;
	}
	intpart += fractpart;

	if (sign) intpart = 0 - intpart;

	return intpart;
}

TempReadStatus TempSensor::readTemp(uint32_t t, bool forceread)
{
	if (!forceread)
		if (!enabled) return TR_DONE;
	//Serial.print("t - "); Serial.print(t);
	//Serial.print(", lastread - "); Serial.print(lastread);
	//Serial.print(". Abs(t - lastread): "); Serial.println(abs(t-lastread));
	if (abs(t - lastread) >= readinterval && status != TR_WAIT)
	{//time to read
		lastread = t;
		//request real read
		dt->requestTemperaturesByAddress(sid.addr);
		status = TR_WAIT;
	}
	else if (status == TR_WAIT && abs(t - lastread) > 2000)//Uzstrigom - pakartokim
	{
		lastread = millis();
		repeat_count++;
		if (repeat_count < 4) {
			//request real read
			Log.error("Uzstrigo temp " + getSensorNameAddr() + " nuskaitymas - pakartojam.");
			delay(1000);
			dt->requestTemperaturesByAddress(sid.addr);
			status = TR_WAIT;
		}
		else {
			Log.error("Tsensor: " + getSensorNameAddr() + " pakartojom 3 kartus. Grazinam sena reiksme " + String(currtemp));
			repeat_count = 0;
			lastdispatch = t;
			status = TR_DONE;
		}
	}
	else if (status == TR_WAIT && abs(t - lastread) > 150)
	{
		if (dt->isConversionAvailable(sid.addr))
		{
			repeat_count = 0;
			float tempC = dt->getTempC(sid.addr);

			if (forceread)
			{
				currtemp = tempC;
				status = TR_DONE;
				return status;
			}

			if (Utilities::nearEqual(tempC, -127.0))
			{
				tempC = currtemp;
				Log.debug("TSensor " + getSensorNameAddr() + " grazino -127.0 Atduodam sena temp: " + String(currtemp));
				status = TR_DONE;
				return status;
			}
			
			avgtemp = Utilities::round2(average.Add(tempC, false));
			readcount++;
			if (readcount % average.GetSize() == 0)
			{
				readcount = 0;
				if (!Utilities::nearEqual(currtemp, avgtemp) || abs(t - lastdispatch) >= MINUTES15)
				{
					lastdispatch = t;
					currtemp = avgtemp;
					DispatchTemperature(this, currtemp);
				}
			}
			status = TR_DONE;
		}
	}
	return status;
}

String TempSensor::getSensorAddr()
{
	String result = "";
	for (byte i = 0; i < 8; i++) {
		if (i > 0) result += ":";
		result += String(sid.addr[i], HEX);
	}
	return result;
}

String TempSensor::getSensorNameAddr() {

	String name = String(sid.name);
	if (name == "") {
		name = "N";
	}
	return getSensorAddr() + " " + name;
}

void TempSensor::reset()
{
	lastread = -360000000;
}

void TempSensor::SetResolution(byte res)
{
	dt->setResolution(sid.addr, res);
}

String TempSensor::GetSensorNameAddr(SensorID * sid)
{
	String name = String(sid->name);
	if (name == "") 
	{
		name = "N";
	}

	String result = "";
	for (byte i = 0; i < 8; i++) {
		if (i > 0) result += ":";
		result += String(sid->addr[i], HEX);
	}

	return "[" + result + " " + name+"]";
}
/*********************************************
*********************************************/
CTempSensors::CTempSensors(byte wire, byte testwire):onewire(wire), dallas(&onewire)
{
	count = 0;
	idx = 0;
	cidx = 0;
	resolution = 9;
}

void CTempSensors::Begin()
{
	dallas.begin(false);
	dallas.setWaitForConversion(false);
	TimeSlice.RegisterTimeSlice(this);
}

TempSensor* CTempSensors::getByAddr(uint8_t* addr)
{
	uint8_t* da;
	for (byte i = 0; i < count; i++) 
	{
		TempSensor* ts = sensors[i];
		if (ts)
		{
			da = ts->getAddr();
			bool found = true;
			for (byte j = 0; j < 8; j++) 
			{
				if (addr[j] != da[j]) 
				{
					found = false;
					break;
				}
			}
			if (found)
				return ts;
		}
		else
		{
			Log.error("TSensor getByAddr() IS NULL.");
			return NULL;
		}
	}
	Log.error("TSensor getByAddr() NOT FOUND.");
	return NULL;
}

TempSensor* CTempSensors::getByName(const char* name)
{
	for (byte i = 0; i < count; i++) 
	{
		TempSensor* ts = sensors[i];
		if (ts)
		{
			if (!strcmp(ts->getName(), name))
			{
				return ts;
			}
		}
		else
		{
			Log.error("TSensor getByName() IS NULL.");
			return NULL;
		}
	}
	Log.error("TSensor getByName() NOT FOUND.");
	return NULL;
}

TempSensor* CTempSensors::getByIndex(int idx)
{
	if (idx >= 0 && idx < MAX_SENSOR_COUNT)
	{
		return sensors[idx];
	}
	return NULL;
}

bool CTempSensors::ScanSensors()
{
	DeviceAddress addr;
	if (dallas.startupSearch(addr))
	{
		SensorFounded(addr);
		//delay(100);
		return true;
	}
	return false;
}

byte CTempSensors::getSensorCount()
{
	return count;
}

void CTempSensors::SensorFounded(uint8_t* da)
{
	TempSensor* ts = sensors.Add();
	if (ts)
	{
		count++;
		ts->SetAddress(da);
		ts->SetDallas(&dallas);
		ts->SetResolution(resolution);
		ts->AddTempChangeHandler(this);
		//sensors[count - 1] = new TempSensor(da, dallas);
		//sensors[count - 1]->SetResolution(resolution);
		//sensors[count - 1]->AddTempChangeHandler(this);
	}
}

void CTempSensors::InitialReadAllTemperatures()
{
	initialread = true;

	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < count; i++)
		{
			bool done = false;
			while (!done)
			{
				TempSensor* ts = sensors[i];
				if (ts)
				{
					switch (ts->readTemp(millis(), true))
					{
					case TR_DONE:
						done = true;
						//delay(100);
						Log.debug("Initial read. Sensor " + String(i) + " temp: " + String(ts->getTemp()));
						break;
					case TR_ERROR:
						done = true;
						//delay(100);
						Log.debug("Error reading sensor " + String(i));
						//do something with error
						break;
					case TR_WAIT:
						delay(10);
						break;
					default:
						break;
					}
				}
				else
					done = true;
			}
		}
		for (int r = 0; r < count; r++)
		{
			TempSensor* ts = sensors[r];
			if (ts)
				ts->reset();
		}
	}
	initialread = false;
}

void CTempSensors::OnTimeSlice()// ReadTemperatures(uint32_t t)
{
	//Log.debug("TempSensorsClass::OnTimeSlice()");
	TempReadStatus trs;
	if (count > 0) 
	{
		trs = sensors[cidx]->readTemp(millis());
		if (trs == TR_DONE)
		{
			cidx++;
			if (cidx >= count) cidx = 0;
			delay(200);
		}
		else if (trs == TR_ERROR) //error reading temp
		{
			Log.error("TempSensor read error. Sensor: " + sensors[cidx]->getSensorAddr() + " - " + String(sensors[cidx]->getName()));
		}
		else if (trs == TR_WAIT)
		{
		}
	}
}

void CTempSensors::SetResolution(byte res)
{
	resolution = constrain(res, 9, 12);
}

void CTempSensors::HandleTemperatureChange(void* Sender, float t)
{
	if (initialread)
		return;
	if (!Utilities::nearEqual(t, -127.0))
	{
		StaticJsonBuffer<200> jbuff;
		JsonObject& root = jbuff.createObject();
		TempSensor* ts = (TempSensor*)Sender;

		root[jKeyTarget] = jTargetTSensor;
		root[jKeyAction] = jValueInfo;
		root[jKeyAddr] = ts->getSensorAddr();
		root[jKeyValue] = t;
		PrintJson.Print(root);
	}
}

CTempSensors TSensors(ONE_WIRE_PIN, 0);