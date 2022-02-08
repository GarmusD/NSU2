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

void Logger::log(String value, bool new_line)
{
	if(!started) return;
	Serial.print("0 " + value);
	if (new_line)
	{
		Serial.print('\n');
	}
}

void Logger::debug(String value)
{
	if(!started) return;
	log("DBG: " + value);
}

void Logger::info(String value)
{
	if(!started) return;
	log("INFO: " + value);
}

void Logger::error(String value)
{
	if(!started) return;
	log("ERROR: " + value);
}

void Logger::newLine()
{
	if (!started) return;
	Serial.print('\n');
	//Serial.println();
}

Logger Log;