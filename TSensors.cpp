#include <math.h>
#include "TSensors.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "SystemStatus.h"
#include "FileManager.h"


const int CTempSensors::CURRENT_CONFIG_VERSION = 2;
const VersionInfo CTempSensors::VINFO = { 'T', 'S', 'N', CURRENT_CONFIG_VERSION };
const char* CTempSensors::CFG_FILE = "tempsens.cfg";


/*********************************************
*********************************************/
/*********************************************
*********************************************/
bool TempSensor::StringToAddr(const char* saddr, uint8_t * addr)
{
	char s[MAX_NAME_LENGTH];
	char* p = s;
	for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) s[i] = 0;
	stpncpy(p, saddr, MAX_NAME_LENGTH - 1);
	char* str;
	uint8_t counter = 0;
	for (uint8_t i = 0; i < 8; i++) 
	{
		if ((str = strtok_r(p, ":", &p)) != NULL) 
		{
			counter++;
			addr[i] = strtol(str, NULL, 16);
		}
	}
	if (counter == 0)
		return true;
	return false;
}

TempSensor::TempSensor()
{
	Reset();
}

void TempSensor::Reset()
{
	Events::Reset();
	errors = 0;
	status = TR_DONE;
	enabled = false;
	currtemp = -127.0;
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

void TempSensor::ApplyConfig(uint8_t cfgPos, const TSensorData & data)
{
	configPos = cfgPos;
	enabled = true;
	setName(data.sid.name);
	SetInterval(data.interval);
}

void TempSensor::SetAddress(const uint8_t* addr)
{
	for (int i = 0; i < 8; i++)
	{
		sid.addr[i] = addr[i];
	}
	char s[64];
	sprintf(s, "Added sensors addr: %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
	Log.debug(s);
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
	strncpy(sid.name, name, MAX_NAME_LENGTH);
}

unsigned int TempSensor::getErrorCount()
{
	return errors;
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
	
	if (abs(t - lastread) >= readinterval && status != TR_WAIT)
	{   //time to read
		lastread = t;
		//request real read
		dt->requestTemperaturesByAddress(sid.addr);
		status = TR_WAIT;
	}
	else if (status == TR_WAIT && abs(t - lastread) > 1000)//Uzstrigom - pakartokim
	{			
			errors++;
			lastread = t;
			status = TR_DONE;
			char s[128];
			snprintf(s, 128, "Tsensor [%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X %s] nenuskaitomas. Klaidu skaicius: %d", 
							sid.addr[0], sid.addr[1], sid.addr[2], sid.addr[3], sid.addr[4], sid.addr[5], sid.addr[6], sid.addr[7], sid.name, errors);
			Log.error(s);
	}
	else if (status == TR_WAIT && abs(t - lastread) > 200)
	{
		if (dt->isConversionAvailable(sid.addr))
		{
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
				char s[128];
				snprintf(s, 128, "Tsensor [%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X %s] grazino -127.0 Atiduodam sena temp: %.1f",
					sid.addr[0], sid.addr[1], sid.addr[2], sid.addr[3], sid.addr[4], sid.addr[5], sid.addr[6], sid.addr[7], sid.name, currtemp);
				Log.debug(s);
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

int TempSensor::getSensorAddr(char * buff, int size)
{
	return snprintf(buff, size, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", sid.addr[0], sid.addr[1], sid.addr[2], sid.addr[3], sid.addr[4], sid.addr[5], sid.addr[6], sid.addr[7]);
}

void TempSensor::reset()
{
	lastread = -360000000;
}

void TempSensor::SetResolution(uint8_t res)
{
	dt->setResolution(sid.addr, res);
}

int TempSensor::GetSensorNameAddr(SensorID* sid, char* buff, uint32_t buff_size)
{
	return snprintf(buff, buff_size, "[%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X %s]", sid->addr[0], sid->addr[1], sid->addr[2], sid->addr[3], sid->addr[4], sid->addr[5], sid->addr[6], sid->addr[7], sid->name);
}

void TempSensor::Simulate(float value)
{
	DispatchTemperature(this, value);
}



/*********************************************
*********************************************/
CTempSensors::CTempSensors(uint8_t wire, uint8_t testwire):onewire(wire), dallas(&onewire)
{
	count = 0;
	idx = 0;
	cidx = 0;
	resolution = 9;
}

void CTempSensors::Begin()
{
	dallas.begin();
	dallas.setWaitForConversion(false);
	TimeSlice.RegisterTimeSlice(this);
}

void CTempSensors::Reset()
{
	sensors.Clear();
	count = 0;
	idx = 0;
	cidx = 0;
	resolution = 9;
	initialread = false;
	Events::Reset();
}

TempSensor* CTempSensors::getByAddr(const uint8_t* addr)
{
	uint8_t* da;
	for (uint8_t i = 0; i < count; i++) 
	{
		TempSensor* ts = sensors[i];
		if (ts)
		{
			da = ts->getAddr();
			bool found = true;
			for (uint8_t j = 0; j < 8; j++) 
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
	for (uint8_t i = 0; i < count; i++) 
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
	if (dallas.getAddress(addr, count))
	{
		SensorFounded(addr);
		//delay(100);
		return true;
	}
	return false;
}

uint8_t CTempSensors::getSensorCount()
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
						char s[64];
						sprintf(s, "Initial read. Sensor %d temp: %.1f", i, ts->getTemp());
						Log.debug(s);
						break;
					case TR_ERROR:
						done = true;
						//delay(100);
						sprintf(s, "Error reading sensor %d", i);
						Log.debug(s);
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
			//delay(200);
		}
		else if (trs == TR_ERROR) //error reading temp
		{
			char sa[64];
			char s[128];
			sensors[cidx]->getSensorAddr(sa, 64);
			sprintf(s, "TempSensor read error. Sensor: %s - %s", sa, sensors[cidx]->getName());
			Log.error(s);
		}
		else if (trs == TR_WAIT)
		{
		}
	}
}

void CTempSensors::SetResolution(uint8_t res)
{
	resolution = constrain(res, 9, 12);
}

void CTempSensors::HandleTemperatureChange(void* Sender, float t)
{
	if (initialread)
		return;
	if (!Utilities::nearEqual(t, -127.0))
	{		
		DynamicJsonDocument	jbuff(512);
		JsonObject root = jbuff.to<JsonObject>();
		TempSensor* ts = (TempSensor*)Sender;
		char saddr[32];
		ts->getSensorAddr(saddr, 32);

		root[jKeyTarget] = jKeyTargetTSensor;
		root[jKeyAction] = jValueInfo;
		root[jKeyTSensorAddr] = saddr;
		root[jKeyValue] = t;
		root[jKeyTSensorErrorCount] = ts->getErrorCount();
		PrintJson.Print(root);
	}
}

void CTempSensors::LoadConfig(void)
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_SENSOR_COUNT; i++)
		{
			TSensorData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					TempSensor* ts = getByAddr(data.sid.addr);
					if (ts)
					{
						ts->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[64];
				sprintf(s, "TSensor config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;	
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_SENSOR_COUNT; i++)
		{
			const TSensorData& data = Defaults.GetDefaultTSensorData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					TempSensor* ts = getByAddr(data.sid.addr);
					if (ts)
					{
						ts->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				char s[64];
				sprintf(s, "TSensor config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_SENSOR_COUNT; i++)
		{
			const TSensorData& data = Defaults.GetDefaultTSensorData(i);
			if (data.Valid)
			{
				TempSensor* ts = getByAddr(data.sid.addr);
				if (ts)
				{
					ts->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool CTempSensors::GetConfigData(uint8_t cfgPos, TSensorData & data)
{
	if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
	{
		uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(TSensorData);
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

bool CTempSensors::ValidateSetupDataSet(ArduinoJson::JsonObject & jo)
{
	if (!jo.containsKey(jKeyEnabled) ||
		!jo.containsKey(jKeyName) ||
		!jo.containsKey(jKeyTSensorAddr) ||
		!jo.containsKey(jKeyTSensorInterval) ||
		!jo.containsKey(jKeyConfigPos)
		)
	{
		//return error
		PrintJson.PrintResultError(jKeyTargetTSensor, jErrorInvalidDataSet);
		return false;
	}
	return true;
}

void CTempSensors::ParseJSON(JsonObject & jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char * action = jo[jKeyAction];
		if (strcmp(action, jKeySetup) == 0)
		{
			if (ValidateSetupDataSet(jo))
			{
				TSensorData data;
				const char* sa = jo[jKeyTSensorAddr];
				bool res = TempSensor::StringToAddr(sa, data.sid.addr);
				if (!res)
				{
					PrintJson.PrintResultError(jKeyTargetTSensor, "invalid_address");
					return;
				}
				data.Valid = jo[jKeyEnabled];
				Utilities::ClearAndCopyString(jo[jKeyName], data.sid.name);
				data.interval = jo[jKeyTSensorInterval];

				//Write settings
				uint8_t cfgPos = jo[jKeyConfigPos];
				if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
				{
					if (FileManager.FileSeek(sizeof(VersionInfo) + cfgPos * sizeof(TSensorData)))
					{
						if (FileManager.FileWriteBuff(&data, sizeof(TSensorData)))
						{
							FileManager.FileClose();
							SystemStatus.SetRebootRequired();
							PrintJson.PrintResultOk(jKeyTargetTSensor, jKeySetup, true);
							return;
						}
					}
				}
				PrintJson.PrintResultError(jKeyTargetTSensor, jErrorFileOpenError);				
			}
		}
		else if (strcmp(action, "simulate") == 0)
		{
			const char * name = jo[jKeyName];
			float val = jo[jKeyValue];
			TempSensor* ts = getByName(name);
			if (ts)
			{
				ts->Simulate(val);
			}
		}
		else
		{
			PrintJson.PrintResultUnknownAction(jKeyTargetTSensor, action);
		}
	}
	else
	{
		PrintJson.PrintResultFormatError();
	}
}

CTempSensors TSensors(ONE_WIRE_PIN, 0);