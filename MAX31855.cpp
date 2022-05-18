#include <SPI.h>
#include "MAX31855.h"
#include "nsu_pins.h"
#include "ArduinoJson.h"
#include "JsonPrinter.h"
#include "SystemStatus.h"

const int CKTypes::CURRENT_CONFIG_VERSION = 1;
const VersionInfo CKTypes::VINFO = { 'K', 'T', 'P', CURRENT_CONFIG_VERSION };
const char* CKTypes::CFG_FILE = "ktype.cfg";

MAX31855::MAX31855(void)
{
	name[0]=0;	
	sec = 5000;
	ctemp = -127;
	ltemp = ctemp;	
}


MAX31855::~MAX31855(void)
{
}

void MAX31855::Begin()
{
	readTemp();
	last_read = millis();
	Timers.AddTimerHandler(this, 0, MINUTES60);
	HandleTimerEvent(0);
	TimeSlice.RegisterTimeSlice(this);
}

void MAX31855::Reset()
{
	name[0] = 0;
	sec = 5000;
	ctemp = -127;
	ltemp = ctemp;
	Events::Reset();
}

void MAX31855::ApplyConfig(uint8_t cfgPos, const KTypeData & data)
{
	SetName(data.Name);	
	SetTempReadInterval(data.interval);
	//Begin();
}

int32_t MAX31855::GetTemp()
{
	return (int)round(ctemp);
}

void MAX31855::OnTimeSlice()// TimeEvent(uint32_t t)
{
	if(Utilities::CheckMillis(last_read, sec))
	{
		readTemp();
	}
}

void MAX31855::SetTempReadInterval(int s)
{
	if(s>0)
	{
		sec = s * 1000;
	}
}

void MAX31855::readTemp()
{
	SPI.beginTransaction(SPISettings());
	digitalWrite(MAX31855_CS, LOW);

	uint8_t r1 = SPI.transfer(0xAA, SPI_CONTINUE);
	uint8_t r2 = SPI.transfer(0xAA, SPI_CONTINUE);
	uint8_t r3 = SPI.transfer(0xAA, SPI_CONTINUE);
	uint8_t r4 = SPI.transfer(0xAA);
	
	digitalWrite(MAX31855_CS, HIGH);
	SPI.endTransaction();

	int value = 0;
	value = (value << 8) + r1;
	value = (value << 8) + r2;
	value = (value << 8) + r3;
	value = (value << 8) + r4;

	value = value >> 18; //shift out all but tc temp data and sign bit
	float temp = floor((value * 0.25)+0.5); //

	//if(temp != ctemp)
	//{
		ctemp = Utilities::round2(average.Add(temp));
		DispatchTemperature(this, ctemp);
	//}
}

void MAX31855::SetName(const char* name)
{
	strlcpy(this->name, name, MAX_NAME_LENGTH);
}

const char* MAX31855::GetName()
{
	return name;
}

//Handlinam Katilo busena ir keiciam sensoriaus nuskaitymo / loginimo laikus
//Handlas priregistruojamas automatiskai registruojant sensoriu katile
void MAX31855::HandleStatusChange(void* Sender, Status value)
{
	switch (value)
	{
	case STATUS_KATILAS_UNKNOWN:
		Timers.ChangeResolution(this, MINUTES60);
		break;
	case STATUS_KATILAS_UZGESES:
		Timers.ChangeResolution(this, MINUTES60);
		break;
	case STATUS_KATILAS_IKURIAMAS:
		//Timers.ChangeResolution(this, MINUTES1);
		Timers.ChangeResolution(this, 1000 * 10);
		break;
	case STATUS_KATILAS_KURENASI:
		//Timers.ChangeResolution(this, MINUTES1);
		Timers.ChangeResolution(this, 1000 * 10);
		break;
	case STATUS_KATILAS_GESTA:
		//Timers.ChangeResolution(this, MINUTES5);
		Timers.ChangeResolution(this, MINUTES1);
		break;
	default:
		break;
	}
}

void MAX31855::HandleTimerEvent(int te_id)
{
	PrintInfo();
}

void MAX31855::PrintInfo()
{
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
	root[jKeyTarget] = jTargetKType;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	root[jKeyValue] = ctemp;
	PrintJson.Print(root);
}

MAX31855* CKTypes::GetByName(const char* name)
{
	MAX31855* max;
	for(int i=0; i<Count(); i++)
	{
		max = Get(i);
		if(strcmp(name, max->GetName()) == 0) return max;
	}
	return NULL;
}

void CKTypes::Begin()
{
	MAX31855* max;
	for (int i = 0; i < Count(); i++)
	{
		max = Get(i);
		if (max) max->Begin();
	}
}

void CKTypes::InitHW()
{
	pinMode(MAX31855_CS, OUTPUT);        //CS is an Output 
	digitalWrite(MAX31855_CS, HIGH);
}

void CKTypes::Reset()
{
	Clear();
}

void CKTypes::LoadConfig()
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_KTYPE_SENSORS; i++)
		{
			KTypeData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					MAX31855* ktp = Add();
					if (ktp)
					{
						ktp->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[128];
				sprintf(s, "KType config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_KTYPE_SENSORS; i++)
		{
			const KTypeData& data = Defaults.GetDefaultKTypeData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					MAX31855* ktp = Add();
					if (ktp)
					{
						ktp->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				char s[128];
				sprintf(s, "KType config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_KTYPE_SENSORS; i++)
		{
			const KTypeData& data = Defaults.GetDefaultKTypeData(i);
			if (data.Valid)
			{
				MAX31855* ktp = Add();
				if (ktp)
				{
					ktp->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool CKTypes::GetConfigData(uint8_t cfgPos, KTypeData & data)
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

bool CKTypes::ValidateSetupDataSet(JsonObject & jo)
{
	return jo.containsKey(jKeyConfigPos) &&
		jo.containsKey(jKeyEnabled) && 
		jo.containsKey(jKeyName) && 
		jo.containsKey(jKTypeInterval);
}

void CKTypes::ParseJson(JsonObject & jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char * action = jo[jKeyAction];
		if (strcmp(action, jKeySetup) == 0)
		{
			if (ValidateSetupDataSet(jo))
			{
				KTypeData data;
				data.Valid = jo[jKeyEnabled];
				Utilities::ClearAndCopyString(jo[jKeyName], data.Name);
				data.interval = jo[jKTypeInterval];

				uint8_t cfgPos = jo[jKeyConfigPos];
				uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
				if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
				{
					if (FileManager.FileSeek(fpos))
					{
						if (FileManager.FileWriteBuff(&data, sizeof(data)))
						{
							FileManager.FileClose();
							SystemStatus.SetRebootRequired();
							PrintJson.PrintResultOk(jTargetKType, action, true);
							return;
						}
					}
					PrintJson.PrintResultError(jTargetKType, jErrorFileWriteError);
					return;
				}
				PrintJson.PrintResultError(jTargetKType, jErrorFileOpenError);
				return;
			}
			else
			{
				PrintJson.PrintResultError(jTargetKType, jErrorInvalidDataSet);
			}
		}
		else 
		{
			PrintJson.PrintResultUnknownAction(jTargetKType, action);
		}
	}
	else
	{
		PrintJson.PrintResultFormatError();
	}
}

CKTypes KTypes;