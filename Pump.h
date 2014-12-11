// Pump.h

#ifndef _PUMP_h
#define _PUMP_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#include <Time.h>
#include "EEPROMCust.h"
#include "Logger.h"

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
#define PUMP_C_LABEL						"Salt"

#define STATE_UNDEFINED						0
#define STATE_INITIALIZED					1
#define STATE_IDLE							2
#define STATE_DAY_ADVANCED					3
#define STATE_DOSING						4
#define STATE_CALIBRATION					5
#define STATE_PIPES_FILL					6
#define STATE_DOSING_COMPLETED				7



#define TIMEOUT_CALIBRATION_DOSE			5000
#define TIMEOUT_PIPES_FILL					((long)20000)

#define TIME_AN_HOUR						((long)60*60*1000)
#define MINUTES_IN_HOUR						((unsigned int)60)

#define DAILY_DOSES_NO						12

#define DAILY_DOSES_MIN_DELAY_HH			1
#define DAILY_DOSES_MIN_DELAY_MM			(DAILY_DOSES_MIN_DELAY_HH * MINUTES_IN_HOUR)

#define DAILY_DOSES_MIN_DOSE				2
#define DAILY_DOSES_RESET_HOUR				1
#define DAILY_DOSES_START_HOUR				8
#define DAILY_DOSES_END_HOUR				20

#define DAILY_DOSE_DEFAULT_PA 				112
#define DAILY_DOSE_DEFAULT_PB 				18
#define DAILY_DOSE_DEFAULT_PC 				160


#define PUMP_PERF_DEFAULT_PA 				1000
#define PUMP_PERF_DEFAULT_PB 				1000
#define PUMP_PERF_DEFAULT_PC 				1000

#define PUMP_DELAY_DEFAULT_PA 				0
#define PUMP_DELAY_DEFAULT_PB 				30
#define PUMP_DELAY_DEFAULT_PC 				45

//EEPROM pump addresses
//remaining doses (ml)
#define EEPROM_ADDR_REMAIN_DOSE_PA			10
#define EEPROM_ADDR_REMAIN_DOSE_PB			12
#define EEPROM_ADDR_REMAIN_DOSE_PC			14

//remaining doses (times)
//#define EEPROM_ADDR_REMAIN_DOSE_PA			210
//#define EEPROM_ADDR_REMAIN_DOSE_PB			212
//#define EEPROM_ADDR_REMAIN_DOSE_PC			214

//daily dose (ml)
#define EEPROM_ADDR_DAILY_DOSE_PA			20
#define EEPROM_ADDR_DAILY_DOSE_PB 			24
#define EEPROM_ADDR_DAILY_DOSE_PC			28

//pumps performance (millis/ml)
#define EEPROM_ADDR_PUMP_PERF_PA			30
#define EEPROM_ADDR_PUMP_PERF_PB			32
#define EEPROM_ADDR_PUMP_PERF_PC			34

//delay between pumps (s)
#define EEPROM_ADDR_PUMP_DELAY_PA			40
#define EEPROM_ADDR_PUMP_DELAY_PB 			42
#define EEPROM_ADDR_PUMP_DELAY_PC			44



//time EEPROM storage addresses (LAST DOSING TIME)
#define EEPROM_ADDR_HH_PA					50
#define EEPROM_ADDR_MM_PA					52
#define EEPROM_ADDR_LAST_DOSE_DAY_PA		54
#define EEPROM_ADDR_LAST_DOSE_MONTH_PA		56
#define EEPROM_ADDR_LAST_DOSE_YEAR_PA		58

#define EEPROM_ADDR_HH_PB					60
#define EEPROM_ADDR_MM_PB					62
#define EEPROM_ADDR_LAST_DOSE_DAY_PB		64
#define EEPROM_ADDR_LAST_DOSE_MONTH_PB		66
#define EEPROM_ADDR_LAST_DOSE_YEAR_PB		68

#define EEPROM_ADDR_HH_PC					70
#define EEPROM_ADDR_MM_PC					72
#define EEPROM_ADDR_LAST_DOSE_DAY_PC		74
#define EEPROM_ADDR_LAST_DOSE_MONTH_PC		76
#define EEPROM_ADDR_LAST_DOSE_YEAR_PC		78


typedef struct  {
	uint8_t Second;
	uint8_t Minute;
	uint8_t Hour;
	uint8_t Day;
	uint8_t Month;
	uint8_t Year;
} 	doseTime_t;

class Pump
{
	protected:
	//pump index
	int PumpIndex;
	//pump Arduino pins
	int PumpEn, PumpIn1, PumpIn2;
	
	
	//eeprom addresses for the pump
	uint8_t EepromAddrRemainDose, EepromAddrDailyDose, EepromAddrPumpPerf, EepromAddrPumpDelay, EepromAddrLastDoseHH, EepromAddrLastDoseMM, EepromAddrLastDoseDay, EepromAddrLastDoseMonth, EepromAddrLastDoseYear;
	LoggerClass logger;
	//pump actual state
	unsigned int PumpState;
	unsigned long PumpStateMillis;
	int LastVolumePumped;
	doseTime_t LastDosingTime;
	doseTime_t NextDosingTime;
	int RemainingDailyDose; //ml
	int AlreadyDosed; //ml
	int DailyDosesNo; //times
	//unsigned int DailyDoseDelay; //minutes between dosages
	unsigned int PumpDelay; //minutes between pumps;
	
	Pump *DependentToPump;
	tmElements_t *CurrentTime;
	
	//dosing definition
	unsigned int DailyDose;
	unsigned int PumpPerf; //ml * 1000ms
	
	//methods
	void EEPROMWriteInt(int, int);
	unsigned int EEPROMReadInt(int);
	
	void defineDailyDosing(int);
	
	void stopPump();
	void startPump();
	void changeState(int state);
	boolean checkDosingStart();
	void doseStart();
	
	boolean advanceDay();
	boolean setNextDosingTime();
	
	unsigned long getDosingMillis();
	void doseEnd(unsigned long);
	
	String to2Digits(String number);
	unsigned int castToUInt(int value);
	
	public:
	Pump();
	Pump(int, tmElements_t&);
	void init();
	void init(Pump&);
	void addDependentPump(Pump&);
	
	int getIndex();
	String getLabel();
	int getState();
	unsigned long getMillisInState();
	
	unsigned int getRemainingDose();
	doseTime_t getNextDosingTime();
	void calibrate();
	void setCalibration(unsigned int);
	void setDosage(unsigned int);
	void fillPipes();
	void setDailyDose(unsigned int);
	int scheduledDose();
	int triggerDose();
	
	void initEEPROM() ;
	String getLastDosingTimeStr();
	String getLastDosingDayStr();
	String getNextDosingTimeStr();
	String getNextDosingDayStr();
	unsigned int getNextDoseMl();
	unsigned int getDoseNo();
	unsigned int getDailyDose();
	unsigned int getPumpPerformance();
	String getEEpromData();
	
};


#endif

