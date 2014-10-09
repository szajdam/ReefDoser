// EEPROMCust.h

#ifndef _EEPROMCUST_h
#define _EEPROMCUST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class EEPROMCustClass
{
 protected:


 public:
	uint8_t readByte(int address);
	void writeByte(int address, uint8_t value);
	void updateByte(int address, uint8_t value);
	
	void writeUInt(int p_address, unsigned int p_value);
	void updateUInt(int p_address, unsigned int p_value);
	unsigned int readUInt(int p_address);
	
};

extern EEPROMCustClass EEPROMCust;

#endif

