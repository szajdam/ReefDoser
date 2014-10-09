#include "EEPROMCust.h"

#include <avr/eeprom.h>
#include "Arduino.h"


uint8_t EEPROMCustClass::readByte(int address)
{
	if ( !eeprom_is_ready () ) {
		//Waiting for EEPROM to become ready
		eeprom_busy_wait ();
	}
	return eeprom_read_byte((unsigned char *) address);
}

void EEPROMCustClass::writeByte(int address, uint8_t value)
{
	if ( !eeprom_is_ready () ) {
		//Waiting for EEPROM to become ready
		eeprom_busy_wait ();
	}
	eeprom_write_byte((unsigned char *) address, value);
}


void EEPROMCustClass::updateByte(int address, uint8_t value)
{
	if ( !eeprom_is_ready () ) {
		//Waiting for EEPROM to become ready
		eeprom_busy_wait ();
	}
	eeprom_write_byte((unsigned char *) address, value);
}

void EEPROMCustClass::writeUInt(int p_address, unsigned int p_value) {
	byte lowByte = ((p_value >> 0) & 0xFF);
	byte highByte = ((p_value >> 8) & 0xFF);

	EEPROMCust.writeByte(p_address, lowByte);
	EEPROMCust.writeByte(p_address + 1, highByte);
}

unsigned int EEPROMCustClass::readUInt(int p_address) {
	byte lowByte = EEPROMCust.readByte(p_address);
	byte highByte = EEPROMCust.readByte(p_address + 1);

	return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

void EEPROMCustClass::updateUInt(int p_address, unsigned int p_value) {
	byte lowByte = ((p_value >> 0) & 0xFF);
	byte highByte = ((p_value >> 8) & 0xFF);

	EEPROMCust.updateByte(p_address, lowByte);
	EEPROMCust.updateByte(p_address + 1, highByte);
}

EEPROMCustClass EEPROMCust;

