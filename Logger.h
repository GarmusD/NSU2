#ifndef Logger_h
#define Logger_h

#include "Arduino.h"
#include "FreeMem.h"

class Logger
{
public:
	Logger(void);
	~Logger(void);
	void begin();
	void log(const char* value, bool new_line = true);
	void debug(const char* value);
	void info(const char* value);
	void error(const char* value);
	//void newLine();
	void memInfo();
private:
	bool started;
};

extern Logger Log;

#endif