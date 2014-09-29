#include "UTFT_ext.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#include "ThermoMeter.h"
#include "Definitions.h"
#include <EEPROM.h>
#include "Pump.h"
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <UTFT.h>
#include <memorysaver.h>
#include <avr/pgmspace.h> 
#include <UTouchCD.h>
#include <UTouch.h>

int x, y;
char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";

unsigned long lastMillis = millis();

unsigned long loopMillis = 1000; //second

ThermoMeter tMeter;



void setup(){
	readTime();
	tMeter.init();
	scrInit();
	drawMainScreen(tMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
	//setRTCTimeFromFile();
}


void loop() {
	drawMillis(lastMillis);
	chooseAction();
	dose();
	
	
	//wait to receive full loop within a second 
	
	if (millis() - lastMillis >= loopMillis) {
		lastMillis = millis();
		readTime();
		drawBar(tMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
		updatePumpsStatus();
	}
 }


