// Pump.h

#ifndef _PUMP_h
#define _PUMP_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#include <Time.h>
#include <EEPROM.h>

#define INDEX_PUMP_A						1
#define INDEX_PUMP_B						2
#define INDEX_PUMP_C						3

//Pump Ports
//Pump A Alkalinity
#define PUMPA_EN							43
#define PUMPA_IN1							45
#define PUMPA_IN2							47
//Pump B Calcium
#define PUMPB_EN							42
#define PUMPB_IN1							44
#define PUMPB_IN2							46
//Pump C SALT
#define PUMPC_EN							43
#define PUMPC_IN1							48
#define PUMPC_IN2							49


#define PUMP_A_LABEL						"Alk"
#define PUMP_B_LABEL						"Ca"
#define PUMP_C_LABEL						"NA"

#define STATE_UNDEFINED						0
#define STATE_INITIALIZED					1
#define STATE_IDLE							2
#define STATE_DOSING						3
#define STATE_CALIBRATION					4
#define STATE_PIPES_FILL					5



#define TIMEOUT_CALIBRATION_DOSE			5000
#define TIMEOUT_PIPES_FILL					(long)20000

#define TIME_AN_HOUR						(long)60*60*1000 //an Hour
#define MINUTES_IN_HOUR						(unsigned int)60 //an Hour

#define DAILY_DOSES							8
#define DAILY_DOSES_DELAY					(long)60*60*1000 //an Hour delay between particular doses
#define DAILY_DOSES_RESET_HOUR				0
#define DAILY_DOSES_START_HOUR				8


#define DAILY_DOSE_DEFAULT_PA 				112 //ALK
#define DAILY_DOSE_DEFAULT_PB 				18 //CA
#define DAILY_DOSE_DEFAULT_PC 				160 //SALT

#define PUMP_PERF_DEFAULT_PA 				1000
#define PUMP_PERF_DEFAULT_PB 				1000
#define PUMP_PERF_DEFAULT_PC 				1000

//EEPROM pump addresses
//remaining doses (ml)
#define EEPROM_ADDR_REMAIN_DOSE_PA			200
#define EEPROM_ADDR_REMAIN_DOSE_PB			202
#define EEPROM_ADDR_REMAIN_DOSE_PC			204

//remaining doses (times)
//#define EEPROM_ADDR_REMAIN_DOSE_PA			210
//#define EEPROM_ADDR_REMAIN_DOSE_PB			212
//#define EEPROM_ADDR_REMAIN_DOSE_PC			214

//daily dose (ml)
#define EEPROM_ADDR_DAILY_DOSE_PA			220
#define EEPROM_ADDR_DAILY_DOSE_PB 			222
#define EEPROM_ADDR_DAILY_DOSE_PC			224

//pumps performance (millis/ml)
#define EEPROM_ADDR_PUMP_PERF_PA			230
#define EEPROM_ADDR_PUMP_PERF_PB			232
#define EEPROM_ADDR_PUMP_PERF_PC			234

//delay between pumps (s)
#define EEPROM_ADDR_PUMP_DELAY_PA_TO_PB 	242
#define EEPROM_ADDR_PUMP_DELAY_PB_TO_PC		244


//time EEPROM storage addresses (LAST DOSING TIME)
#define EEPROM_ADDR_HH_PA					1000
#define EEPROM_ADDR_MM_PA					1002

#define EEPROM_ADDR_HH_PB					1010
#define EEPROM_ADDR_MM_PB					1012

#define EEPROM_ADDR_HH_PC					1020
#define EEPROM_ADDR_MM_PC					1022

#define EEPROM_ADDR_LAST_DOSE_DAY			1100

typedef struct  {
	uint8_t Second;
	uint8_t Minute;
	uint8_t Hour;
	uint8_t Day;
} 	doseTime_t;

class Pump
{
	protected:
	//pump index
	int PumpIndex;
	//pump Arduino pins
	int PumpEn, PumpIn1, PumpIn2;
	
	
	//eeprom addresses for the pump
	int EepromAddrRemainDose, EepromAddrDailyDose, EepromAddrPumpPerf, EepromAddrPumpDelay, EepromAddrHH, EepromAddrMM, EepromAddrLastDoseDay;
	
	//pump actual state
	int PumpState;
	unsigned long PumpStateMillis;
	unsigned int LastVolumePumped;
	doseTime_t LastDosingTime;
	doseTime_t NextDosingTime;
	unsigned int RemainingDailyDose; //ml
	//unsigned int RemainingDailyDosesNo; //times
	unsigned int PumpDelay; //minutes;
	
	Pump *DependentToPump;
	tmElements_t *CurrentTime;
	
	//dosing definition
	int DailyDose;
	unsigned long PumpPerf;
	
	//methods
	void EEPROMWriteInt(int, int);
	unsigned int EEPROMReadInt(int);
	
	void stopPump();
	void startPump();
	void changeState(int state);
	boolean checkDosingStart();
	void doseStart();
	
	unsigned long checkDosingEnd();
	void doseEnd(unsigned long);
	
	String to2Digits(String number);
	
	public:
	Pump();
	Pump(int, tmElements_t&);
	void init();
	void init(Pump&);
	
	int getIndex();
	String getLabel();
	int getState();
	unsigned long getMillisInState();
	
	int getRemainingDose();
	doseTime_t getNextDosingTime();
	String getNextDosingTimeStr();
	void calibrate();
	void setCalibration(int);
	void setDosage(int);
	void fillPipes();
	void setDailyDose(int);
	int dose();
	
	void advanceDay();
	
	void initEEPROM() ;
};


#endif

