// Logger.h

#ifndef _LOGGER_h
#define _LOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class LoggerClass
{
 protected:
 String Source;
 boolean loggingAvailable;
 String Msg;


 public:

	LoggerClass(String source);
	LoggerClass();
	void flush();
	void appendLog(String msg);
	void log(String msg);
};


#endif

