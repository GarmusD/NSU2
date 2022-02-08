#ifndef Logger_h
#define Logger_h

#include "Arduino.h"

class Logger
{
public:
	Logger(void);
	~Logger(void);
	void begin();
	//void log(String value);
	void log(String value, bool new_line = true);
	void debug(String value);
	void info(String value);
	void error(String value);
	void newLine();
private:
	bool started;
};

extern Logger Log;

#endif