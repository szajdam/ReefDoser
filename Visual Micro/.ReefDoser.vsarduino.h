/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Mega w/ ATmega2560 (Mega 2560), Platform=avr, Package=arduino
*/

#define __AVR_ATmega2560__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

//
//
Pump* getPumpARef();
Pump* getPumpBRef();
Pump* getPumpCRef();
void dose();
void updatePumpsStatus();
void scrInit();
void drawButton(int x1, int y1, int x2, int y2);
void pressButton(int x1, int y1, int x2, int y2);
void drawBar(float temp, String time, String date);
void drawPump(int x1, int y1, int x2, int y2, String label);
void drawPumpWStatus(int x1, int y1, int x2, int y2, String label, String nextDoseTimeStr, int remainingDose);
void updatePumpStatus(int x1, int y1, int x2, int y2, String nextDoseTimeStr, int remainingDose);
void updatePumpPicture(int x1, int y1, int x2, int y2, String labelPump, int colorR, int colorG, int colorB);
void drawMillis(unsigned long currMilis);
void drawLog(String logMsg);
void drawMainScreen(float temp, String time, String date);
void readMainScreen(int x, int y);
void drawMainMenuScreen(float temp, String time, String date);
void readMainMenuScreen(int x, int y);
void drawPumpsMenuScreen(float temp, String time, String date);
void readPumpMenuScreen(int x, int y);
void drawNumKeyScreen(String label);
void drawNumKeyResult();
void readNumKeyScreen(int x, int y);
void readPumpCalibScreen(int x, int y);
void chooseAction();
void chooseActionMain(int x, int y);
void chooseActionMainMenu(int x, int y);
void chooseActionPumpMenu(int x, int y);
void chooseActionPumpsCalibration(int x, int y);
void chooseActionTimeSet(int x, int y);
void chooseActionDateSet(int x, int y);
void chooseActionNumKeyPumpsCalibrate(int x, int y);
void chooseActionNumKeyPumpsSetup(int x, int y);
bool getFileDate(const char *str);
bool getFileTime(const char *str);
void readTime();
void setRTCTimeFromFile();
String getCurrentTimeStr();
String timeToString(tmElements_t time);
String getCurrentDateStr();
String dateToString(tmElements_t date);
String to2Digits(String number);

#include "D:\Private\Arduino\arduino-1.5.6-r2\hardware\arduino\avr\variants\mega\pins_arduino.h" 
#include "D:\Private\Arduino\arduino-1.5.6-r2\hardware\arduino\avr\cores\arduino\arduino.h"
#include "D:\Private\Projekty\ReefDoser\ReefDoser.ino"
#include "D:\Private\Projekty\ReefDoser\Definitions.h"
#include "D:\Private\Projekty\ReefDoser\Pump.cpp"
#include "D:\Private\Projekty\ReefDoser\Pump.h"
#include "D:\Private\Projekty\ReefDoser\Screens.ino"
#include "D:\Private\Projekty\ReefDoser\ThermoMeter.cpp"
#include "D:\Private\Projekty\ReefDoser\ThermoMeter.h"
#include "D:\Private\Projekty\ReefDoser\TimeUtils.ino"
