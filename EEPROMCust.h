// EEPROMCust.h

#ifndef _EEPROMCUST_h
#define _EEPROMCUST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define OW_EEPROM_ADDR  0x50

class EEPROMCustClass
{
 protected:
	void i2c_eeprom_write_byte(unsigned int eeaddress, byte data);
	void i2c_eeprom_write_page(unsigned int eeaddresspage, byte* data, byte length );
	void i2c_eeprom_read_buffer(unsigned int eeaddress, byte *buffer, int length );
	byte i2c_eeprom_read_byte(unsigned int eeaddress );
	void I2CWriteUInt (unsigned int eeaddress, unsigned int data);
	unsigned int I2CReadUInt (unsigned int eeaddress);

 public:
	EEPROMCustClass();
	uint8_t readByte(int address);
	void writeByte(int address, uint8_t value);
	void updateByte(int address, uint8_t value);
	
	void writeUInt(int p_address, unsigned int p_value);
	void updateUInt(int p_address, unsigned int p_value);
	unsigned int readUInt(int p_address);

	
	
};

extern EEPROMCustClass EEPROMCust;

#endif

