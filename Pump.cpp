#include "Pump.h"
Pump::Pump() {
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
		EepromAddrPumpDelay = -1;
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
	changeState(STATE_UNDEFINED);
	
}



void Pump::initEEPROM() {
	EEPROMWriteInt(EepromAddrDailyDose, DailyDose);
	EEPROMWriteInt(EepromAddrHH, 0);
	EEPROMWriteInt(EepromAddrMM, 0);
	EEPROMWriteInt(EepromAddrLastDoseDay, 0);
	EEPROMWriteInt(EepromAddrPumpDelay, 0);
	EEPROMWriteInt(EepromAddrRemainDose, DailyDose);
	EEPROMWriteInt(EepromAddrPumpPerf, 1000);
	
}
void Pump::init(){
	pinMode(PumpEn, OUTPUT);
	pinMode(PumpIn1, OUTPUT);
	pinMode(PumpIn2, OUTPUT);
	stopPump();
	
	PumpPerf = (EEPROMReadInt(EepromAddrPumpPerf)>0?EEPROMReadInt(EepromAddrPumpPerf):1000);
	DailyDose = (EEPROMReadInt(EepromAddrDailyDose)>0?EEPROMReadInt(EepromAddrDailyDose):DailyDose);
	RemainingDailyDose = EEPROMReadInt(EepromAddrRemainDose);
	//RemainingDailyDosesNo = EEPROMReadInt(EepromAddrNoOfRemainDoses);
	PumpDelay = EepromAddrPumpDelay > 0?EEPROMReadInt(EepromAddrPumpDelay):0;
		
	LastDosingTime.Day = EEPROMReadInt(EepromAddrLastDoseDay);
	LastDosingTime.Hour = EEPROMReadInt(EepromAddrHH);
	LastDosingTime.Minute = EEPROMReadInt(EepromAddrMM);
	
	//next dosing - last dosing a day ago (doser switched off for a day)
	if(LastDosingTime.Day<(*CurrentTime).Day) {
		NextDosingTime = LastDosingTime;
		advanceDay();
	}
	//last dosing at the same day - remainder to be dosed
	else if(LastDosingTime.Day == (*CurrentTime).Day) {
		unsigned long delayHH = ((DAILY_DOSES_DELAY) / (TIME_AN_HOUR));
		unsigned long delayMM = round((DAILY_DOSES_DELAY) % (TIME_AN_HOUR));
		NextDosingTime.Day = LastDosingTime.Day;
		NextDosingTime.Hour = LastDosingTime.Hour + delayHH;
		NextDosingTime.Minute = LastDosingTime.Minute + delayMM;
	}
	
	
	changeState(STATE_INITIALIZED);
}
void Pump::init(Pump &dependentPump){
	DependentToPump = &dependentPump;
	init();
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
	return "EMPTY";
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
	changeState(STATE_CALIBRATION);
	startPump();
	delay(TIMEOUT_CALIBRATION_DOSE);
	stopPump();
	changeState(STATE_IDLE);
}
void Pump::setCalibration(int pumpPerformance) {
	PumpPerf = pumpPerformance;
	EEPROMWriteInt(EepromAddrPumpPerf, pumpPerformance);
}
void Pump::setDosage(int dosage) {
	DailyDose = dosage;
	EEPROMWriteInt(EepromAddrDailyDose, dosage);
}
void Pump::fillPipes() {
	changeState(STATE_PIPES_FILL);
	startPump();
	delay(TIMEOUT_PIPES_FILL);
	stopPump();
	changeState(STATE_IDLE);
}
boolean Pump::dose(){
	advanceDay();
	//start dosing
	if(checkDosingStart()) {
		doseStart();
	}
	else {
		unsigned long dosedMillis = checkDosingEnd();
		if(dosedMillis!=0) {
			doseEnd(dosedMillis);
		}
	}
	
	if(PumpState == STATE_DOSING) 
		return true;
	else 
		return false;
}
boolean Pump::checkDosingStart() {
	if(PumpState == STATE_INITIALIZED || PumpState == STATE_IDLE) {
		//check to dose remainder
		if(RemainingDailyDose > 0){
			//check date and time
			if(NextDosingTime.Day == (*CurrentTime).Day 
				&& NextDosingTime.Hour <= (*CurrentTime).Hour 
				&& NextDosingTime.Minute <= (*CurrentTime).Minute) {
				return true;
			}
		}
	}
	return false;
}
unsigned long Pump::checkDosingEnd() {
	if(PumpState == STATE_DOSING) {
		unsigned long currentMillis = millis();
		unsigned long millisToDose = ((DailyDose * PumpPerf) / DAILY_DOSES);
		if(((currentMillis - PumpStateMillis) >= millisToDose) 
			|| (millisToDose < (RemainingDailyDose * PumpPerf)  && currentMillis - PumpStateMillis >= (RemainingDailyDose * PumpPerf))){
			return (currentMillis - PumpStateMillis);
		}
		else {
			return 0;
		}
	}
	return 0;
}
void Pump::doseStart() {
	changeState(STATE_DOSING);
	startPump();
}
void Pump::doseEnd(unsigned long dosedMillis) {
	stopPump();
	changeState(STATE_IDLE);
	LastVolumePumped = (dosedMillis/PumpPerf);
	//set remaining Dose
	RemainingDailyDose = RemainingDailyDose - LastVolumePumped;
	EEPROMWriteInt(EepromAddrRemainDose, RemainingDailyDose);
	
	LastDosingTime.Day = (*CurrentTime).Day;
	LastDosingTime.Hour = (*CurrentTime).Hour;
	LastDosingTime.Minute = (*CurrentTime).Minute;
	//set last dose day
	if(EEPROMReadInt(EepromAddrLastDoseDay) != LastDosingTime.Day) {
		EEPROMWriteInt(EepromAddrLastDoseDay, LastDosingTime.Day);
	}
	//set last dose time
	if(EEPROMReadInt(EepromAddrHH) != LastDosingTime.Hour) {
		EEPROMWriteInt(EepromAddrHH, LastDosingTime.Hour);
	}
	if(EEPROMReadInt(EepromAddrMM) != LastDosingTime.Minute) {
		EEPROMWriteInt(EepromAddrMM, LastDosingTime.Minute);
	}
	
	if(RemainingDailyDose > 0) {
		NextDosingTime.Day = LastDosingTime.Day; //current day
		int delayHH = ((DAILY_DOSES_DELAY) / (TIME_AN_HOUR));
		int delayMM = ((DAILY_DOSES_DELAY) % (TIME_AN_HOUR));
		
		NextDosingTime.Hour = LastDosingTime.Hour + delayHH;
		NextDosingTime.Minute = LastDosingTime.Minute + delayMM + (LastDosingTime.Minute - DependentToPump->LastDosingTime.Minute) < PumpDelay? PumpDelay - (LastDosingTime.Minute - DependentToPump->LastDosingTime.Minute):PumpDelay;
		
	}
	
}
void Pump::startPump() {
	//digitalWrite(LED_NOTIF, HIGH);
	digitalWrite(PumpEn, HIGH);
	digitalWrite(PumpIn1, HIGH);
	digitalWrite(PumpIn2, LOW);
}
void Pump::stopPump() {
	digitalWrite(PumpIn1, LOW);
	digitalWrite(PumpIn2, LOW);
	digitalWrite(PumpEn, LOW);
}
void Pump::advanceDay() {
	if(NextDosingTime.Day < (*CurrentTime).Day && LastDosingTime.Day < (*CurrentTime).Day && (*CurrentTime).Hour >= DAILY_DOSES_RESET_HOUR) {
		RemainingDailyDose = (RemainingDailyDose + DailyDose);
		EEPROMWriteInt(EepromAddrRemainDose, RemainingDailyDose);
		
		NextDosingTime.Day = (*CurrentTime).Day; //current day
		
		NextDosingTime.Hour = ((*CurrentTime).Hour >= DAILY_DOSES_START_HOUR ? (*CurrentTime).Hour : DAILY_DOSES_START_HOUR);
		NextDosingTime.Minute = ((*CurrentTime).Hour >= DAILY_DOSES_START_HOUR ? (*CurrentTime).Minute + 1 : 0) ;
	}
}
int Pump::getRemainingDose() {
	return RemainingDailyDose;
}
doseTime_t Pump::getNextDosingTime() {
	return NextDosingTime;
}
String Pump::getNextDosingTimeStr() {
	String timeStr = to2Digits(String((int) NextDosingTime.Hour));
	timeStr = timeStr + ":";
	timeStr = timeStr +  to2Digits(String((int) NextDosingTime.Minute));;
	return timeStr;

}

String Pump::to2Digits(String number) {
	if (number.length() < 2) {
		return "0" + number;
	}
	return String(number);
}



void Pump::setDailyDose(int value) {
	DailyDose = value;
	EEPROMWriteInt(EepromAddrDailyDose, DailyDose);
}
void Pump::EEPROMWriteInt(int p_address, int p_value) {
	byte lowByte = ((p_value >> 0) & 0xFF);
	byte highByte = ((p_value >> 8) & 0xFF);

	EEPROM.write(p_address, lowByte);
	EEPROM.write(p_address + 1, highByte);
}
unsigned int Pump::EEPROMReadInt(int p_address) {
	byte lowByte = EEPROM.read(p_address);
	byte highByte = EEPROM.read(p_address + 1);

	return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}




