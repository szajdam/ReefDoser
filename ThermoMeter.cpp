#include "ThermoMeter.h"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);



void ThermoMeter::init()
{
	tempSensor.begin();
	tempSensor.getAddress(tempSensorAddr, 0);
	tempSensor.setResolution(tempSensorAddr, 9);
	
	this->readTemp();
}

void ThermoMeter::readTemp() {
	tempSensor.requestTemperatures();
	temperature = tempSensor.getTempC(tempSensorAddr);
	lastReadMillis = millis();
}

float ThermoMeter::getTemperature() {
	if(millis() - lastReadMillis > TEMP_UPDATE_FREQUENCY) {
		this->readTemp();
	}
	return temperature;
} 


