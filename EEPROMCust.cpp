#include "EEPROMCust.h"

#include <avr/eeprom.h>
#include "Arduino.h"
#include <Wire.h>

boolean UseOneWireEEPROM = true;


EEPROMCustClass::EEPROMCustClass() {
	
	if (UseOneWireEEPROM){
		Wire.begin();
	}
	
}

void EEPROMCustClass::i2c_eeprom_write_byte(unsigned int eeaddress, byte data) {
	int rdata = data;
	Wire.beginTransmission(OW_EEPROM_ADDR);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.write(rdata);
	Wire.endTransmission();
}

void EEPROMCustClass::I2CWriteUInt (unsigned int eeaddress, unsigned int data) {
	byte lowByte = ((data >> 0) & 0xFF);
	byte highByte = ((data >> 8) & 0xFF);

	Wire.beginTransmission(OW_EEPROM_ADDR);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.write(lowByte);
	Wire.write(highByte);
	Wire.endTransmission();
}

unsigned int EEPROMCustClass::I2CReadUInt (unsigned int eeaddress) {
	Wire.beginTransmission(OW_EEPROM_ADDR);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(OW_EEPROM_ADDR, 2);
	byte lowByte = 0;
	byte highByte = 0;
	if (Wire.available()) {
		lowByte = Wire.read();
		highByte = Wire.read();
	}
	return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

// WARNING: address is a page address, 6-bit end will wrap around
// also, data can be maximum of about 30 bytes, because the Wire library has a buffer of 32 bytes
void EEPROMCustClass::i2c_eeprom_write_page(unsigned int eeaddresspage, byte* data, byte length ) {
	Wire.beginTransmission(OW_EEPROM_ADDR);
	Wire.write((int)(eeaddresspage >> 8)); // MSB
	Wire.write((int)(eeaddresspage & 0xFF)); // LSB
	byte c;
	for ( c = 0; c < length; c++)
	Wire.write(data[c]);
	Wire.endTransmission();
}

byte EEPROMCustClass::i2c_eeprom_read_byte(unsigned int eeaddress ) {
	byte rdata = 0xFF;
	Wire.beginTransmission(OW_EEPROM_ADDR);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(OW_EEPROM_ADDR,1);
	if (Wire.available()) rdata = Wire.read();
	return rdata;
}

// maybe let's not read more than 30 or 32 bytes at a time!
void EEPROMCustClass::i2c_eeprom_read_buffer(unsigned int eeaddress, byte *buffer, int length ) {
	Wire.beginTransmission(OW_EEPROM_ADDR);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(OW_EEPROM_ADDR,length);
	int c = 0;
	for ( c = 0; c < length; c++ )
	if (Wire.available()) buffer[c] = Wire.read();
}

uint8_t EEPROMCustClass::readByte(int address)
{
	if(UseOneWireEEPROM) {
		return i2c_eeprom_read_byte(address);
	}
	else {
		if ( !eeprom_is_ready () ) {
		//Waiting for EEPROM to become ready
		eeprom_busy_wait ();
		}
		return eeprom_read_byte((uint8_t *) address);
	}
	
}

void EEPROMCustClass::writeByte(int address, uint8_t value)
{
	if(UseOneWireEEPROM) {
		i2c_eeprom_write_byte(address, value);
	}
	else {
		if ( !eeprom_is_ready () ) {
			//Waiting for EEPROM to become ready
			eeprom_busy_wait ();
		}
		eeprom_write_byte((uint8_t *) address, value);
	}
}


void EEPROMCustClass::updateByte(int address, uint8_t value) {
	writeByte(address, value);
}

void EEPROMCustClass::writeUInt(int p_address, unsigned int p_value) {
	if(UseOneWireEEPROM) {
		I2CWriteUInt(p_address, p_value);
	}
	else {
		byte lowByte = ((p_value >> 0) & 0xFF);
		byte highByte = ((p_value >> 8) & 0xFF);
		
		EEPROMCust.writeByte(p_address, lowByte);
		EEPROMCust.writeByte(p_address + 1, highByte);
	}
}

unsigned int EEPROMCustClass::readUInt(int p_address) {
	if(UseOneWireEEPROM) {
		return I2CReadUInt(p_address);
	}
	else {
		byte lowByte = EEPROMCust.readByte(p_address);
		byte highByte = EEPROMCust.readByte(p_address + 1);

		return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
	}
}

void EEPROMCustClass::updateUInt(int p_address, unsigned int p_value) {
	writeUInt(p_address, p_value);
}

EEPROMCustClass EEPROMCust;

