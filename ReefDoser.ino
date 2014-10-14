#include "Logger.h"
#include "EEPROMCust.h"
#include "UTFT_ext.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#include "ThermoMeter.h"
#include "Definitions.h"
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

unsigned long LastMillis = millis();

unsigned long LoopMillis = (unsigned long)10*1000; //10 seconds

ThermoMeter TMeter;



void setup(){
	readTime();
	TMeter.init();
	scrInit();
	drawMainScreen(TMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
	delay(1000);
	updatePumpsStatus();
}


void loop() {
	drawMillis(millis());
	chooseAction();
	dose();
	
	//wait to receive full loop within a second 
	
	if (millis() - LastMillis >= LoopMillis) {
		LastMillis = millis();
		readTime();
		drawBar(TMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
		updatePumpsStatus();
	}
 }


