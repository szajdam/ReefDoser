#include "Pump.h"

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
		
		DailyDose = EEPROM_ADDR_DAILY_DOSE_PA;
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
		
		DailyDose = EEPROM_ADDR_DAILY_DOSE_PB;
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
		
		DailyDose = EEPROM_ADDR_DAILY_DOSE_PC;
		break;
	}
	
	CurrentTime = &currentTime;
	changeState(STATE_UNDEFINED);
	
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
		advanceDay();
	}
	//last dosing at the same day - remainder to be dosed
	else if(LastDosingTime.Day == (*CurrentTime).Day) {
		int delayHH = round(DAILY_DOSES_DELAY / TIME_AN_HOUR);
		int delayMM = round(DAILY_DOSES_DELAY % TIME_AN_HOUR);
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
			if(NextDosingTime.Day == (*CurrentTime).Day && NextDosingTime.Hour >= (*CurrentTime).Hour && NextDosingTime.Minute >= (*CurrentTime).Minute) {
				return true;
			}
		}
	}
	return false;
}
unsigned long Pump::checkDosingEnd() {
	if(PumpState == STATE_DOSING) {
		unsigned long currentMillis = millis();
		unsigned long millisToDose = DailyDose*PumpPerf/DAILY_DOSES;
		if((currentMillis - PumpStateMillis >= millisToDose) || 
			//dose remainder
			(millisToDose < RemainingDailyDose && currentMillis - PumpStateMillis >= RemainingDailyDose)){
			return currentMillis - PumpStateMillis;
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
	LastVolumePumped = round(dosedMillis/PumpPerf);
	//set remaining Dose
	RemainingDailyDose =- LastVolumePumped;
	EEPROMWriteInt(EepromAddrRemainDose, RemainingDailyDose);
	
	LastDosingTime = *CurrentTime;
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
		int delayHH = round(DAILY_DOSES_DELAY / TIME_AN_HOUR);
		int delayMM = DAILY_DOSES_DELAY % TIME_AN_HOUR;
		
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
	if(LastDosingTime.Day == (*CurrentTime).Day && (*CurrentTime).Hour >= DAILY_DOSES_RESET_HOUR) {
		RemainingDailyDose =+ DailyDose;
		EEPROMWriteInt(EepromAddrRemainDose, RemainingDailyDose);
		
		NextDosingTime.Day = LastDosingTime.Day; //current day
		
		NextDosingTime.Hour = (*CurrentTime).Hour >= DAILY_DOSES_START_HOUR?(*CurrentTime).Hour:DAILY_DOSES_START_HOUR;
		NextDosingTime.Minute = PumpDelay;
	}
}
int Pump::getRemainingDose() {
	return RemainingDailyDose;
}
tmElements_t Pump::getNextDosingDate() {
	return NextDosingTime;
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




