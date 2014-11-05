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
		EepromAddrPumpDelay = EEPROM_ADDR_PUMP_DELAY_PA;
		EepromAddrLastDoseHH = EEPROM_ADDR_HH_PA;
		EepromAddrLastDoseMM = EEPROM_ADDR_MM_PA;
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
		EepromAddrPumpDelay = EEPROM_ADDR_PUMP_DELAY_PB;
		EepromAddrLastDoseHH = EEPROM_ADDR_HH_PB;
		EepromAddrLastDoseMM = EEPROM_ADDR_MM_PB;
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
		EepromAddrPumpDelay = EEPROM_ADDR_PUMP_DELAY_PC;
		EepromAddrLastDoseHH = EEPROM_ADDR_HH_PC;
		EepromAddrLastDoseMM = EEPROM_ADDR_MM_PC;
		EepromAddrLastDoseDay = EEPROM_ADDR_LAST_DOSE_DAY;
		
		DailyDose = DAILY_DOSE_DEFAULT_PC;
		break;
	}
	
	CurrentTime = &currentTime;
	
	logger = LoggerClass("Pump.cpp" + getLabel());
	
	AlreadyDosed = 0;
	LastVolumePumped = 0;
	
	this->changeState(STATE_UNDEFINED);
	
}



void Pump::initEEPROM() {
	switch (PumpIndex)	{	
		case INDEX_PUMP_A:
		DailyDose = DAILY_DOSE_DEFAULT_PA;
		PumpDelay = PUMP_DELAY_DEFAULT_PA;
		break;
		case INDEX_PUMP_B:
		DailyDose = DAILY_DOSE_DEFAULT_PB;
		PumpDelay = PUMP_DELAY_DEFAULT_PB;
		break;
		case INDEX_PUMP_C:
		DailyDose = DAILY_DOSE_DEFAULT_PC;
		PumpDelay = PUMP_DELAY_DEFAULT_PC;
		break;
	}
	EEPROMCust.writeUInt(EepromAddrDailyDose, DailyDose);
	EEPROMCust.writeUInt(EepromAddrLastDoseHH, 1);
	EEPROMCust.writeUInt(EepromAddrLastDoseMM, 1);
	EEPROMCust.writeUInt(EepromAddrLastDoseDay, CurrentTime->Day - 1);
	EEPROMCust.writeUInt(EepromAddrPumpDelay, PumpDelay);
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
	
	logger.appendLog("Pump::init() PumpPerf");
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
	LastDosingTime.Hour = EEPROMCust.readUInt(EepromAddrLastDoseHH);
	LastDosingTime.Minute = EEPROMCust.readUInt(EepromAddrLastDoseMM);
	
	logger.appendLog("Pump::init() LastDosingTime.Day");
	logger.appendLog((String)LastDosingTime.Day);
	logger.appendLog(".Hour");
	logger.appendLog((String)LastDosingTime.Hour);
	logger.appendLog(".Minute");
	logger.appendLog((String)LastDosingTime.Minute);
	logger.flush();
	
	
	
	if(!this->advanceDay()) {
		logger.log("Pump::init() this->advanceDay() == FALSE");
		
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
		EEPROMCust.writeUInt(EepromAddrRemainDose, castToUInt(RemainingDailyDose));
		
		this->defineDailyDosing(RemainingDailyDose);
		
		LastDosingTime.Day = CurrentTime->Day;
		LastDosingTime.Hour = 0;
		LastDosingTime.Minute = 0;
		
		EEPROMCust.writeUInt(EepromAddrLastDoseDay, LastDosingTime.Day);
		EEPROMCust.writeUInt(EepromAddrLastDoseHH, LastDosingTime.Hour);
		EEPROMCust.writeUInt(EepromAddrLastDoseMM, LastDosingTime.Minute);
		
		if(!setNextDosingTime()) {
			logger.log("Pump::advanceDay() No dosing for Today");
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
		if(CurrentTime->Hour < (uint8_t)(DAILY_DOSES_END_HOUR - 2)) {
			/*dailydose/max(minimaldosage;(dailydose/20-7))*/
			DailyDosesNo =  (unsigned int)round((float)dailyDose / max(DAILY_DOSES_MIN_DOSE, round((float)dailyDose / (DAILY_DOSES_END_HOUR - max(DAILY_DOSES_START_HOUR, CurrentTime->Hour)))));
		}
		else {
			DailyDosesNo = 2;
		}
	}
	else {
		DailyDosesNo = 0;
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
void Pump::setCalibration(unsigned int mililitersDosed) { //ml*10	
	PumpPerf = round(((float)TIMEOUT_CALIBRATION_DOSE / mililitersDosed) * 10);
	EEPROMCust.writeUInt(EepromAddrPumpPerf, PumpPerf);
}
void Pump::setDosage(unsigned int dosage) {
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
		unsigned long doseMillis = this->getDosingMillis();
		this->doseStart();
		delay(doseMillis);
		this->doseEnd(doseMillis);
	}
// 	else {
// 		
// 		if(dosedMillis > 0) {
// 			this->doseEnd(dosedMillis);
// 		}
// 	}
	
	return PumpState;
}

int Pump::triggerDose() {
	if(RemainingDailyDose > 0) {
		unsigned long doseMillis = this->getDosingMillis();
		this->doseStart();
		delay(doseMillis);
		this->doseEnd(doseMillis);
	}
	
	return PumpState;
}

boolean Pump::checkDosingStart() {
	if(PumpState >= STATE_INITIALIZED && PumpState <= STATE_DAY_ADVANCED) {
		/*check to dose remainder*/
		if(RemainingDailyDose > 0){
			/*check date and time*/
			if(NextDosingTime.Day == CurrentTime->Day 
				&& NextDosingTime.Hour <= CurrentTime->Hour 
				&& NextDosingTime.Minute <= CurrentTime->Minute) {
				return true;
			}
		}
	}
	return false;
}
unsigned long Pump::getDosingMillis() {
	//if(PumpState == STATE_DOSING) {
		//unsigned long currentMillis = millis();
		unsigned long millisToDose = min((unsigned long)round(((double)DailyDose * PumpPerf) / DailyDosesNo), (unsigned long)castToUInt(RemainingDailyDose) * PumpPerf);  /*remaning dose can be lower than calculated dosage;*/
		//unsigned long millisDiff = (currentMillis - PumpStateMillis);
		
		logger.appendLog("Pump::getDosingMillis() millisToDose");
		logger.appendLog((String)millisToDose);
		//logger.appendLog("millisDiff");
		//logger.appendLog((String)millisDiff);
		logger.flush();
		return millisToDose;
		//if(millisDiff >= millisToDose) {
		//	logger.log("Pump::getDosingMillis() millisDiff >= millisToDose");
		//	return millisDiff;
		//}
		//else {
		//	return 0;
		//}
	//}
	//return 0;
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
	
	logger.appendLog("Pump::doseEnd() LastVolumePumped");
	logger.appendLog((String)LastVolumePumped);
	logger.appendLog("AlreadyDosed");
	logger.appendLog((String)AlreadyDosed);
	
	//set remaining Dose
	RemainingDailyDose = RemainingDailyDose - LastVolumePumped;
	EEPROMCust.updateUInt(EepromAddrRemainDose, castToUInt(RemainingDailyDose));
	
	logger.appendLog("RemainingDailyDose");
	logger.appendLog((String)RemainingDailyDose);
	logger.flush();
	
	LastDosingTime.Day = CurrentTime->Day;
	LastDosingTime.Hour = CurrentTime->Hour;
	LastDosingTime.Minute = CurrentTime->Minute;
	
	//set last dose day
	if(EEPROMCust.readUInt(EepromAddrLastDoseDay) != LastDosingTime.Day) {
		EEPROMCust.updateUInt(EepromAddrLastDoseDay, LastDosingTime.Day);
	}
	//set last dose time
	if(EEPROMCust.readUInt(EepromAddrLastDoseHH) != LastDosingTime.Hour) {
		EEPROMCust.updateUInt(EepromAddrLastDoseHH, LastDosingTime.Hour);
	}
	if(EEPROMCust.readUInt(EepromAddrLastDoseMM) != LastDosingTime.Minute) {
		EEPROMCust.updateUInt(EepromAddrLastDoseMM, LastDosingTime.Minute);
	}

	if(RemainingDailyDose > 0) {
		logger.log("Pump::doseEnd() RemainingDailyDose > 0");
		if (!this->setNextDosingTime()) {
			logger.log("Pump::doseEnd() !this->setNextDosingTime()");
			this->changeState(STATE_DOSING_COMPLETED);	
		}
	}
	else {		
		NextDosingTime.Day = CurrentTime->Day + 1;
		NextDosingTime.Hour = DAILY_DOSES_START_HOUR;
		NextDosingTime.Minute = PumpDelay;
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

	/*calculate dosing delay*/

	NextDosingTime.Day = CurrentTime->Day;
	if(CurrentTime->Minute + 1 < (uint8_t)PumpDelay) {
		NextDosingTime.Hour = (uint8_t)max(max(DAILY_DOSES_START_HOUR, CurrentTime->Hour), LastDosingTime.Hour + DAILY_DOSES_MIN_DELAY_HH);
		NextDosingTime.Minute = (uint8_t)PumpDelay;
	}
	else {
		NextDosingTime.Hour = max(max(DAILY_DOSES_START_HOUR, CurrentTime->Hour + 1), LastDosingTime.Hour + DAILY_DOSES_MIN_DELAY_HH);
		NextDosingTime.Minute = (uint8_t)PumpDelay;
	}
	logger.appendLog("Pump::setNextDosingTime() NextDosingTime.Hour");
	logger.appendLog((String)NextDosingTime.Hour);
	logger.appendLog(".Minute");
	logger.appendLog((String)NextDosingTime.Minute);
	logger.flush();
	
	if(NextDosingTime.Hour >= 23) {
		NextDosingTime.Day = CurrentTime->Day + 1;
		NextDosingTime.Hour = (uint8_t)DAILY_DOSES_START_HOUR;
		NextDosingTime.Minute = (uint8_t)PumpDelay;
		return false;
	}
	
	return true;
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
	String data = "AD:";
	data = data + AlreadyDosed;
	data = data + " LVP:";
	data = data + LastVolumePumped;
	//data = data + " DD";
	//data = data + EEPROMCust.readUInt(EepromAddrDailyDose);
	return data;
}

unsigned int Pump::getNextDoseMl() {
	return min((unsigned int)round((float)DailyDose / DailyDosesNo), castToUInt(RemainingDailyDose));
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

void Pump::setDailyDose(unsigned int value) {
	DailyDose = value;
	EEPROMCust.writeUInt(EepromAddrDailyDose, DailyDose);
}

unsigned int Pump::castToUInt(int value) {
	if(value > 0) {
		return value;
	}
	return 0;
}
