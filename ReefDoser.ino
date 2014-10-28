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

unsigned long BigLoopLastMillis = millis();
unsigned long SmallLoopLastMillis = millis();

unsigned long BigLoopMillis = (unsigned long)10*1000; //10 seconds
unsigned long SmallLoopMillis = (unsigned long)500; //1/2 second

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
	if(millis() - SmallLoopLastMillis >= SmallLoopMillis) {
		SmallLoopLastMillis = millis();
		drawMillis(millis());
		chooseAction();
		dose();
	
		//wait to receive full loop within a second 
	
		if (millis() - BigLoopLastMillis >= BigLoopMillis) {
			BigLoopLastMillis = millis();
			readTime();
			drawBar(TMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
			updatePumpsStatus();
		
		}
	
	}
}


