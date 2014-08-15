// ThermoMeter.h

#ifndef _THERMOMETER_h
#define _THERMOMETER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <OneWire.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS				19
#define TEMP_UPDATE_FREQUENCY		(long)60*1000 //A MINUTE

class ThermoMeter
{
 protected:
	float temperature;
	DeviceAddress tempSensorAddr;
	unsigned long lastReadMillis;
	
	void readTemp();

 public:
	
	void init();
	float getTemperature();
};


#endif

