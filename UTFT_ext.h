// UTFT_ext.h

#ifndef _UTFT_EXT_h
#define _UTFT_EXT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <UTFT.h>

class UTFT_ext: public UTFT
{
 protected:


 public:
	UTFT_ext(byte model, int RS, int WR, int CS, int RST) : UTFT(model, RS, WR, CS, RST) {}
	void	lcdOff();
	void	lcdOn();
};


#endif

