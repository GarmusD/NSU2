#include <SPI.h>
#include "MAX31855.h"
#include "nsu_pins.h"
#include "ArduinoJson.h"
#include "JsonPrinter.h"


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

void MAX31855::Init()
{
	pinMode(MAX31855_CS, OUTPUT);        //CS is an Output 
	digitalWrite(MAX31855_CS, HIGH);
}

void MAX31855::Begin()
{
	readTemp();
	last_read = millis();
	Timers.AddTimerHandler(this, 0, MINUTES60);
	HandleTimerEvent(0);
	TimeSlice.RegisterTimeSlice(this);
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
	digitalWrite(MAX31855_CS, LOW);

	byte r1 = SPI.transfer(0xAA, SPI_CONTINUE);
	byte r2 = SPI.transfer(0xAA, SPI_CONTINUE);
	byte r3 = SPI.transfer(0xAA, SPI_CONTINUE);
	byte r4 = SPI.transfer(0xAA);
	
	digitalWrite(MAX31855_CS, HIGH);

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
		Timers.ChangeResolution(this, MINUTES1);
		break;
	case STATUS_KATILAS_KURENASI:
		Timers.ChangeResolution(this, MINUTES1);
		break;
	case STATUS_KATILAS_GESTA:
		Timers.ChangeResolution(this, MINUTES5);
		break;
	default:
		break;
	}
}

void MAX31855::HandleTimerEvent(int te_id)
{
	PrintInfo();
}

String MAX31855::GetInfoString()
{
	String str = "INFO: KTYPE: " + String(name) + " " + String((int)ctemp);
	return str;
}

void MAX31855::PrintInfo()
{
	StaticJsonBuffer<512> jBuff;
	JsonObject& root = jBuff.createObject();
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


CKTypes KTypes;