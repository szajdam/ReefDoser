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
unsigned long MidLoopLastMillis = millis();
//unsigned long SmallLoopLastMillis = millis();

#define BIG_LOOP_MILLIS		(unsigned long)5*1000
#define MID_LOOP_MILLIS		(unsigned int)1000
//#define SMALL_LOOP_MILLIS	(unsigned long)100

ThermoMeter TMeter;



void setup(){
	readTime();
	TMeter.init();
	scrInit();
	delay(1000);
	drawMainScreen(TMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
	updatePumpsStatus();
}


void loop() {
	if(millis() < MidLoopLastMillis) { //if overflow of millis
		//SmallLoopLastMillis = millis();
		MidLoopLastMillis = millis();
		BigLoopLastMillis = millis();
	}
	//if(millis() - SmallLoopLastMillis >= SMALL_LOOP_MILLIS) {
		//SmallLoopLastMillis = millis();
				
		if(millis() - MidLoopLastMillis >= MID_LOOP_MILLIS) {
			MidLoopLastMillis = millis();
			chooseAction();
			drawBar(TMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
			readTime();
			
			if (millis() - BigLoopLastMillis >= BIG_LOOP_MILLIS) {
				BigLoopLastMillis = millis();
				dose();
				updatePumpsStatus();
				drawMillis(millis());				
			}
		}
	
	//}
}


