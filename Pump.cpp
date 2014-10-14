#include "Pump.h"



Pump::Pump() {
	logger = LoggerClass();
}

Pump::Pump(int p_pumpIndex, tmElements_t& currentTime) {
	PumpIndex = p_pumpIndex;
	switch (PumpIndex)
	{
		case INDEX_PUMP_A:
		PumpEn = PUMPA_EN;
		PumpIn1 = PUMPA_IN1;
		PumpIn2 = PUMPA_IN2;
		
		EepromAddrRemainDose = EEPROM_ADDR_REMAIN_DOSE_PA;
		EepromAddrDailyDose = EEPROM_ADDR_DAILY_DOSE_PA;
		EepromAddrPumpPerf = EEPROM_ADDR_PUMP_PERF_PA;
		EepromAddrPumpDelay = EEPROM_ADDR_PUMP_DELAY_PA_TO_PB;
		EepromAddrHH = EEPROM_ADDR_HH_PA;
		EepromAddrMM = EEPROM_ADDR_MM_PA;
		EepromAddrLastDoseDay = EEPROM_ADDR_LAST_DOSE_DAY;
		
		DailyDose = DAILY_DOSE_DEFAULT_PA;
		break;
		case INDEX_PUMP_B:
		PumpEn = PUMPB_EN;
		PumpIn1 = PUMPB_IN1;
		PumpIn2 = PUMPB_IN2;
		
		EepromAddrRemainDose = EEPROM_ADDR_REMAIN_DOSE_PB;
		EepromAddrDailyDose = EEPROM_ADDR_DAILY_DOSE_PB;
		EepromAddrPumpPerf = EEPROM_ADDR_PUMP_PERF_PB;
		EepromAddrPumpDelay = EEPROM_ADDR_PUMP_DELAY_PA_TO_PB;
		EepromAddrHH = EEPROM_ADDR_HH_PB;
		EepromAddrMM = EEPROM_ADDR_MM_PB;
		EepromAddrLastDoseDay = EEPROM_ADDR_LAST_DOSE_DAY;
		
		DailyDose = DAILY_DOSE_DEFAULT_PB;
		break;
		case INDEX_PUMP_C:
		PumpEn = PUMPC_EN;
		PumpIn1 = PUMPC_IN1;
		PumpIn2 = PUMPC_IN2;
		
		EepromAddrRemainDose = EEPROM_ADDR_REMAIN_DOSE_PC;
		EepromAddrDailyDose = EEPROM_ADDR_DAILY_DOSE_PC;
		EepromAddrPumpPerf = EEPROM_ADDR_PUMP_PERF_PC;
		EepromAddrPumpDelay = EEPROM_ADDR_PUMP_DELAY_PB_TO_PC;
		EepromAddrHH = EEPROM_ADDR_HH_PC;
		EepromAddrMM = EEPROM_ADDR_MM_PC;
		EepromAddrLastDoseDay = EEPROM_ADDR_LAST_DOSE_DAY;
		
		DailyDose = DAILY_DOSE_DEFAULT_PC;
		break;
	}
	
	CurrentTime = &currentTime;
	
	logger = LoggerClass("Pump.cpp" + getLabel());
	
	this->changeState(STATE_UNDEFINED);
	
}



void Pump::initEEPROM() {
	switch (PumpIndex)	{	
		case INDEX_PUMP_A:
		DailyDose = DAILY_DOSE_DEFAULT_PA;
		break;
		case INDEX_PUMP_B:
		DailyDose = DAILY_DOSE_DEFAULT_PB;
		break;
		case INDEX_PUMP_C:
		DailyDose = DAILY_DOSE_DEFAULT_PC;
		break;
	}
	EEPROMCust.writeUInt(EepromAddrDailyDose, DailyDose);
	EEPROMCust.writeUInt(EepromAddrHH, 1);
	EEPROMCust.writeUInt(EepromAddrMM, 1);
	EEPROMCust.writeUInt(EepromAddrLastDoseDay, CurrentTime->Day - 1);
	if(EepromAddrPumpDelay > 0){
		EEPROMCust.writeUInt(EepromAddrPumpDelay, 15);
	}
	EEPROMCust.writeUInt(EepromAddrRemainDose, 0);
	EEPROMCust.writeUInt(EepromAddrPumpPerf, 1000);
	delay(50);
	init();
	
}
void Pump::init(){
	pinMode(PumpEn, OUTPUT);
	pinMode(PumpIn1, OUTPUT);
	pinMode(PumpIn2, OUTPUT);
	this->stopPump();
	
	PumpPerf = EEPROMCust.readUInt(EepromAddrPumpPerf);
	DailyDose = EEPROMCust.readUInt(EepromAddrDailyDose);
	RemainingDailyDose = EEPROMCust.readUInt(EepromAddrRemainDose);
	PumpDelay = EEPROMCust.readUInt(EepromAddrPumpDelay);
	
	logger.appendLog("PumpPerf");
	logger.appendLog((String)PumpPerf);
	logger.flush();	
	logger.appendLog("DailyDose");
	logger.appendLog((String)DailyDose);
	logger.flush();
	logger.appendLog("RemainingDailyDose");
	logger.appendLog((String)RemainingDailyDose);
	logger.flush();
	logger.appendLog("PumpDelay");
	logger.appendLog((String)PumpDelay);
	logger.flush();
	
	LastDosingTime.Day = EEPROMCust.readUInt(EepromAddrLastDoseDay);
	LastDosingTime.Hour = EEPROMCust.readUInt(EepromAddrHH);
	LastDosingTime.Minute = EEPROMCust.readUInt(EepromAddrMM);
	
	logger.appendLog("LastDosingTime.Day");
	logger.appendLog((String)LastDosingTime.Day);
	logger.appendLog(".Hour");
	logger.appendLog((String)LastDosingTime.Hour);
	logger.appendLog(".Minute");
	logger.appendLog((String)LastDosingTime.Minute);
	logger.flush();
	
	
	
	if(!this->advanceDay()) {
	
		this->defineDailyDosing(RemainingDailyDose);
		this->setNextDosingTime();
		
	}
	
	changeState(STATE_INITIALIZED);
}
void Pump::init(Pump &dependentPump){
	DependentToPump = &dependentPump;
	this->init();
}
boolean Pump::advanceDay() {
	if(LastDosingTime.Day < CurrentTime->Day ) {
		
		AlreadyDosed = 0;
		RemainingDailyDose = DailyDose + RemainingDailyDose;
		EEPROMCust.writeUInt(EepromAddrRemainDose, RemainingDailyDose);
		
		this->defineDailyDosing(RemainingDailyDose);
		
		LastDosingTime.Day = CurrentTime->Day;
		EEPROMCust.writeUInt(EepromAddrLastDoseDay, LastDosingTime.Day);
		
		uint8_t nextDosingHH = 0;
		unsigned int nextDosingMM = 0;
		/*=TIME(MAX(HOUR(curr_time); DAILY_DOSES_START_HOUR);IF((HOUR(curr_time) >= DAILY_DOSES_START_HOUR);MINUTE(curr_time) + 1;pump_delay_a);0)
		=TIME(HOUR(E3);MINUTE(E3) + pump_delay_b;0)*/
		
		if(DependentToPump == NULL || (DependentToPump->LastDosingTime.Day < CurrentTime->Day)) {
			nextDosingHH = max(CurrentTime->Hour, DAILY_DOSES_START_HOUR);
			nextDosingMM = (CurrentTime->Hour >= DAILY_DOSES_START_HOUR ?
			(CurrentTime->Minute + 5) :
			0);
		}
		else {
			nextDosingHH = DependentToPump->NextDosingTime.Hour;
			nextDosingMM = DependentToPump->NextDosingTime.Minute + PumpDelay;
			
		}
		/*60 minutes handling*/
		if(nextDosingMM < MINUTES_IN_HOUR) {
			NextDosingTime.Hour = nextDosingHH;
			NextDosingTime.Minute = nextDosingMM;	
		}
		else {
			NextDosingTime.Hour = (nextDosingHH + floor((float)nextDosingMM / MINUTES_IN_HOUR));
			NextDosingTime.Minute = (nextDosingMM % MINUTES_IN_HOUR);	
		}
		
		if(NextDosingTime.Hour < 24) {
			NextDosingTime.Day = CurrentTime->Day;
		}
		else { //setup for the next day
			NextDosingTime.Day = CurrentTime->Day + 1;
			
			NextDosingTime.Hour = DAILY_DOSES_START_HOUR;
			NextDosingTime.Minute = PumpDelay;
		}
		
		this->changeState(STATE_DAY_ADVANCED);
		return true;
	}
	else {
		return false;
	}
}
void Pump::addDependentPump(Pump &dependentPump){
	DependentToPump = &dependentPump;
}

void Pump::defineDailyDosing(int dailyDose) {
	if(dailyDose > 0) {
		if(CurrentTime->Hour < DAILY_DOSES_END_HOUR) {
			/*dailydose/max(minimaldosage;(dailydose/20-7))*/
			DailyDosesNo =  round((float)dailyDose / max(DAILY_DOSES_MIN_DOSE, round((float)dailyDose / (DAILY_DOSES_END_HOUR - max(DAILY_DOSES_START_HOUR, CurrentTime->Hour)))));
			DailyDoseDelay = max(DAILY_DOSES_MIN_DELAY, (round((((float)DAILY_DOSES_END_HOUR - max(DAILY_DOSES_START_HOUR, CurrentTime->Hour))/DailyDosesNo)*MINUTES_IN_HOUR)));
		}
		else {
			DailyDosesNo = 2;
			DailyDoseDelay = DAILY_DOSES_MIN_DELAY;
		}
	}
	else {
		DailyDosesNo = 0;
		DailyDoseDelay = DAILY_DOSES_MIN_DELAY;
	}
	
}

int Pump::getIndex() {
	return PumpIndex;
}
String Pump::getLabel()
{
	switch (PumpIndex)
	{
		case INDEX_PUMP_A:
		return PUMP_A_LABEL;
		case INDEX_PUMP_B:
		return PUMP_B_LABEL;
		case INDEX_PUMP_C:
		return PUMP_C_LABEL;
	}
	return "NA";
}
int Pump::getState() {
	return PumpState;
}
void Pump::changeState(int state) {
	PumpStateMillis = millis();
	PumpState = state;
}
unsigned long Pump::getMillisInState() {
	return millis() - PumpStateMillis;
}
void Pump::calibrate() {
	this->changeState(STATE_CALIBRATION);
	this->startPump();
	delay(TIMEOUT_CALIBRATION_DOSE);
	this->stopPump();
	this->changeState(STATE_IDLE);
}
void Pump::setCalibration(int pumpPerformance) {
	PumpPerf = pumpPerformance;
	EEPROMCust.writeUInt(EepromAddrPumpPerf, pumpPerformance);
}
void Pump::setDosage(int dosage) {
	DailyDose = dosage;
	EEPROMCust.writeUInt(EepromAddrDailyDose, dosage);
}
void Pump::fillPipes() {
	this->changeState(STATE_PIPES_FILL);
	this->startPump();
	delay(TIMEOUT_PIPES_FILL);
	this->stopPump();
	this->changeState(STATE_IDLE);
}
int Pump::scheduledDose(){
	if(CurrentTime->Hour >= DAILY_DOSES_RESET_HOUR) {
		this->advanceDay();
	}
	/*start dosing*/
	if(this->checkDosingStart()) {
		this->doseStart();
	}
	else {
		unsigned long dosedMillis = this->checkDosingEnd();
		if(dosedMillis > 0) {
			this->doseEnd(dosedMillis);
		}
	}
	
	return PumpState;
}

int Pump::triggerDose() {
	if(RemainingDailyDose > 0) {
		this->doseStart();
	}
	
	return PumpState;
}

boolean Pump::checkDosingStart() {
	if(PumpState == STATE_INITIALIZED || PumpState == STATE_DAY_ADVANCED || PumpState == STATE_IDLE) {
		/*check to dose remainder*/
		if(RemainingDailyDose > 0){
			/*check date and time*/
			if(NextDosingTime.Day == CurrentTime->Day 
				&& NextDosingTime.Hour == CurrentTime->Hour 
				&& NextDosingTime.Minute <= CurrentTime->Minute) {
				return true;
			}
			else if(NextDosingTime.Day == CurrentTime->Day
				&& NextDosingTime.Hour < CurrentTime->Hour) {
				return true;
			}
		}
	}
	return false;
}
unsigned long Pump::checkDosingEnd() {
	if(PumpState == STATE_DOSING) {
		unsigned long currentMillis = millis();
		unsigned long millisToDose = min((unsigned long)round((double)(DailyDose * PumpPerf) / DailyDosesNo), (unsigned long)RemainingDailyDose * PumpPerf);  /*remaning dose can be lower than calculated dosage;*/
		unsigned long millisDiff = (currentMillis - PumpStateMillis);
		if(millisDiff >= millisToDose) {
			return millisDiff;
		}
		else {
			return 0;
		}
	}
	return 0;
}
void Pump::doseStart() {
	this->changeState(STATE_DOSING);
	this->startPump();
}
void Pump::doseEnd(unsigned long dosedMillis) {
	this->stopPump();
	this->changeState(STATE_IDLE);
	LastVolumePumped = (unsigned int)round((float)dosedMillis/PumpPerf);
	AlreadyDosed = AlreadyDosed + LastVolumePumped;
	
	//set remaining Dose
	RemainingDailyDose = RemainingDailyDose - LastVolumePumped;
	EEPROMCust.updateUInt(EepromAddrRemainDose, RemainingDailyDose);
	
	LastDosingTime.Day = CurrentTime->Day;
	LastDosingTime.Hour = CurrentTime->Hour;
	LastDosingTime.Minute = CurrentTime->Minute;
	
	//set last dose day
	if(EEPROMCust.readUInt(EepromAddrLastDoseDay) != LastDosingTime.Day) {
		EEPROMCust.updateUInt(EepromAddrLastDoseDay, LastDosingTime.Day);
	}
	//set last dose time
	if(EEPROMCust.readUInt(EepromAddrHH) != LastDosingTime.Hour) {
		EEPROMCust.updateUInt(EepromAddrHH, LastDosingTime.Hour);
	}
	if(EEPROMCust.readUInt(EepromAddrMM) != LastDosingTime.Minute) {
		EEPROMCust.updateUInt(EepromAddrMM, LastDosingTime.Minute);
	}

	if(RemainingDailyDose > 0) {
		if (!this->setNextDosingTime()) {
			this->changeState(STATE_DOSING_COMPLETED);	
		}
	}
	else {
		this->changeState(STATE_DOSING_COMPLETED);
	}
	
}
void Pump::startPump() {
	digitalWrite(PumpEn, HIGH);
	digitalWrite(PumpIn1, HIGH);
	digitalWrite(PumpIn2, LOW);
}
void Pump::stopPump() {
	digitalWrite(PumpIn1, LOW);
	digitalWrite(PumpIn2, LOW);
	digitalWrite(PumpEn, LOW);
}
boolean Pump::setNextDosingTime() {

	/*=TIME(HOUR(C6);MINUTE(C6)+dose_del_b+MAX(pump_delay_b-IF(HOUR(TIME(last_dose+dose_del_b))>HOUR(E6);60-MINUTE(E6)+MINUTE(last_dose+dose_del_b));MINUTE(last_dose+dose_del_b))-MINUTE(E6));0);0)*/
	
	uint8_t nextDoseHH = LastDosingTime.Hour;
	uint8_t nextDoseMM = LastDosingTime.Minute;
	
	/*calculate dosing delay*/
	if((nextDoseMM + DailyDoseDelay) >= MINUTES_IN_HOUR) {
		nextDoseHH = nextDoseHH + floor((float)(nextDoseMM + DailyDoseDelay) / MINUTES_IN_HOUR);
		nextDoseMM = ((nextDoseMM + DailyDoseDelay) % MINUTES_IN_HOUR);
	}
	else {
		nextDoseHH = nextDoseHH;
		nextDoseMM = (nextDoseMM + DailyDoseDelay);
	}
	
	uint8_t depNextDosingHH = DependentToPump->NextDosingTime.Hour;
	uint8_t depNextDosingMM = DependentToPump->NextDosingTime.Minute;
	
	/*calculate pump dependency delay*/
	if(nextDoseHH > depNextDosingHH){
		unsigned int tempNextDoseMM = nextDoseMM + max(PumpDelay - (MINUTES_IN_HOUR - depNextDosingMM + nextDoseMM), 0);
		if(tempNextDoseMM >= MINUTES_IN_HOUR) {
			nextDoseHH = nextDoseHH + floor((float)tempNextDoseMM / MINUTES_IN_HOUR);
			nextDoseMM = (tempNextDoseMM % MINUTES_IN_HOUR);
		}
		else {
			nextDoseHH = nextDoseHH;
			nextDoseMM = tempNextDoseMM;
		}
	}
	else if(nextDoseHH < depNextDosingHH){
		if((MINUTES_IN_HOUR - nextDoseMM + depNextDosingMM) >= PumpDelay 
			|| nextDoseHH < (depNextDosingHH + 1)) {
			/*nothing*/
		}
		else {
			if(depNextDosingMM + PumpDelay >= MINUTES_IN_HOUR) {
				nextDoseHH = depNextDosingHH + floor((float)(depNextDosingMM + PumpDelay) / MINUTES_IN_HOUR);
				nextDoseMM = ((depNextDosingMM + PumpDelay) % MINUTES_IN_HOUR);
			}
			else {
				nextDoseHH = depNextDosingHH;
				nextDoseMM = depNextDosingMM + PumpDelay;
			}
		}
	}
	else { /*nextDoseHH == depNextDosingHH*/
		unsigned int tempNextDoseMM = nextDoseMM + max(PumpDelay - (nextDoseMM - depNextDosingMM), 0);
		if(tempNextDoseMM >= MINUTES_IN_HOUR) {
			nextDoseHH = nextDoseHH + floor((float)tempNextDoseMM / MINUTES_IN_HOUR);
			nextDoseMM = (tempNextDoseMM % MINUTES_IN_HOUR);
		}
		else {
			nextDoseHH = nextDoseHH;
			nextDoseMM = tempNextDoseMM + tempNextDoseMM;
		}
	}
	if(nextDoseHH < 24) {
		NextDosingTime.Day = CurrentTime->Day;
		NextDosingTime.Hour = nextDoseHH;
		NextDosingTime.Minute = nextDoseMM;
		return true;
	}
	else {
		NextDosingTime.Day = CurrentTime->Day + 1;
		NextDosingTime.Hour = DAILY_DOSES_START_HOUR;
		NextDosingTime.Minute = PumpDelay;
	}
	return false;
}

unsigned int Pump::getRemainingDose() {
	return RemainingDailyDose;
}

unsigned int Pump::getDoseNo() {
	return DailyDosesNo;
}

unsigned int Pump::getDailyDose() {
	return DailyDose;
}

unsigned int Pump::getPumpPerformance() {
	return PumpPerf;
}

String Pump::getEEpromData() {
	String data = "R";
	data = data + EEPROMCust.readUInt(EepromAddrRemainDose);
	//data = data + " PP";
	//data = data + EEPROMCust.readUInt(EepromAddrPumpPerf);
	//data = data + " DD";
	//data = data + EEPROMCust.readUInt(EepromAddrDailyDose);
	return data;
}

unsigned int Pump::getNextDoseMl() {
	return min((unsigned int)round((float)DailyDose / DailyDosesNo), RemainingDailyDose);
}

String Pump::getLastDosingTimeStr() {
	String timeStr = to2Digits(String((int) LastDosingTime.Hour));
	timeStr = timeStr + ":";
	timeStr = timeStr +  to2Digits(String((int) LastDosingTime.Minute));
	return timeStr;
}

String Pump::getLastDosingDayStr() {
	String timeStr = to2Digits(String((int) LastDosingTime.Day));
	return timeStr;
}

String Pump::getNextDosingTimeStr() {
	String timeStr = to2Digits(String((int) NextDosingTime.Hour));
	timeStr = timeStr + ":";
	timeStr = timeStr +  to2Digits(String((int) NextDosingTime.Minute));
	return timeStr;

}
String Pump::getNextDosingDayStr() {
	String timeStr = to2Digits(String((int) NextDosingTime.Day));
	return timeStr;

}

doseTime_t Pump::getNextDosingTime() {
	return NextDosingTime;
}

String Pump::to2Digits(String number) {
	if (number.length() < 2) {
		return "0" + number;
	}
	return String(number);
}

void Pump::setDailyDose(int value) {
	DailyDose = value;
	EEPROMCust.writeUInt(EepromAddrDailyDose, DailyDose);
}
