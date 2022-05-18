#include "Logger.h"


Logger::Logger(void)
{
	started = false;
}


Logger::~Logger(void)
{
}

void Logger::begin()
{
	started = true;
}

void Logger::log(const char* value, bool new_line)
{
	if(!started) return;
	Serial.print("0 ");
	Serial.print(value);
	if (new_line)
	{		
		Serial.print('\n');
	}
}

void Logger::debug(const char* value)
{
	if(!started) return;
	Serial.print("0 DBG: ");
	Serial.println(value);
}

void Logger::info(const char* value)
{
	if(!started) return;
	Serial.print("0 INFO: ");
	Serial.println(value);
}

void Logger::error(const char* value)
{
	if(!started) return;
	Serial.print("0 ERROR: ");
	Serial.println(value);
}

/*
void Logger::newLine()
{
	if (!started) return;
	Serial.print('\n');
	//Serial.println();
}
*/

void Logger::memInfo()
{
	info("************** MEMORY INFO **************");
	MemInfo minfo = FreeMem::GetMemInfo();
	char value[256];
	sprintf(value, "Dynamic ram used: %d", minfo.DynamicRamUsed);
	info(value);
	sprintf(value, "Program static ram used: %d", minfo.StaticRamUsed);
	info(value);
	sprintf(value, "Stack ram used: %d", minfo.StackRamUsed);
	info(value);
	sprintf(value, "Free mem: %d", minfo.FreeMem);
	info(value);
	info("*********** END OF MEMORY INFO **********");	
}

Logger Log;