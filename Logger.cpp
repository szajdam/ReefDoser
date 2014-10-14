
#include "Logger.h"

LoggerClass::LoggerClass() {
	loggingAvailable = false;
}

LoggerClass::LoggerClass(String source) {
	
	Source = source;
	loggingAvailable = false;
	if(loggingAvailable) {
		Serial.begin(9600);
		Serial.println(Source);
	}
}

void LoggerClass::appendLog(String msg) {
	if(loggingAvailable) {
		Msg = Msg + msg;
		Msg = Msg + " ";
	}
}

void LoggerClass::flush(){
	if(loggingAvailable) {
		String logMsg = Source;
		logMsg = logMsg + " ";
		logMsg = logMsg + millis();
		logMsg = logMsg + " ";
		logMsg = logMsg + Msg;
		Serial.println(logMsg);
		Msg = "";
	}
}



