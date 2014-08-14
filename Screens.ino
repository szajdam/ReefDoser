#define MAIN_SCREEN					0

#define MAIN_MENU_SCREEN			1

#define PUMPS_MENU_SCREEN			10

#define TIME_SET_MENU_SCREEN		20

#define DATE_SET_MENU_SCREEN		30

#define PUMP_PRE_CALIBRATE_A 		111
#define PUMP_PRE_CALIBRATE_B 		112
#define PUMP_PRE_CALIBRATE_C 		113

#define NUM_KEY_PUMP_CALIBRATE_A 	111
#define NUM_KEY_PUMP_CALIBRATE_B 	112
#define NUM_KEY_PUMP_CALIBRATE_C 	113

#define PUMP_PIPES_FILL_A 			111
#define PUMP_PIPES_FILL_B 			112
#define PUMP_PIPES_FILL_C 			113

#define NUM_KEY_PUMP_SET_DOSE_A 	131
#define NUM_KEY_PUMP_SET_DOSE_B 	132
#define NUM_KEY_PUMP_SET_DOSE_C 	133

#define NUM_KEY_PUMP_SET_DELAY_B 	142
#define NUM_KEY_PUMP_SET_DELAY_C 	143

#define NUM_KEY_TIME_SET_HOURS		211
#define NUM_KEY_TIME_SET_MINUTES	212
#define NUM_KEY_TIME_SET_SECONDS	213

#define NUM_KEY_DATE_SET_YEAR		311
#define NUM_KEY_DATE_SET_MONTH		312
#define NUM_KEY_DATE_SET_DAY		313


int previousMenu = -1;
int currentMenu = -1;
int choosenMenu = -1;

int numKeyResult = -1;

UTFT    lcd(ITDB32S,38,39,40,41);
UTouch  touch( 6, 5, 4, 3, 2);

Pump pumpA(INDEX_PUMP_A, currentTime);
Pump pumpB(INDEX_PUMP_B, currentTime);
Pump pumpC(INDEX_PUMP_C, currentTime);


extern uint8_t BigFont[]; //16x16
extern uint8_t SmallFont[]; //SmallFont[] 8x12

Pump* getPumpARef() {
	return &pumpA;
}
Pump* getPumpBRef() {
	return &pumpB;
}
Pump* getPumpCRef() {
	return &pumpC;
}
void dose() {
	if(pumpA.dose()) {
		updatePumpPicture(10, 40, 40, 70, pumpA.getLabel(), 0, 255, 0);
	}
	else {
		updatePumpPicture(10, 40, 40, 70, pumpA.getLabel(), 0, 0, 255);
	}
	if(pumpB.dose()) {
		updatePumpPicture(10, 90, 40, 120, pumpB.getLabel(), 0, 255, 0);
	}
	else {
		updatePumpPicture(10, 90, 40, 120, pumpB.getLabel(), 0, 0, 255);
	}
	if(pumpC.dose()) {
		updatePumpPicture(10, 140, 40, 170, pumpC.getLabel(), 0, 255, 0);
	}
	else {
		updatePumpPicture(10, 140, 40, 170, pumpC.getLabel(), 0, 0, 255);
	}
}
void updatePumpsStatus() {
	updatePumpStatus(10, 40, 40, 70, pumpA.getNextDosingDate(), pumpA.getRemainingDose());
	updatePumpStatus(10, 90, 40, 120, pumpB.getNextDosingDate(), pumpB.getRemainingDose());
	updatePumpStatus(10, 140, 40, 170, pumpC.getNextDosingDate(), pumpC.getRemainingDose());
}
//layout
// void lcdOn(){
// 	lcd.lcdOn();	
// }
//  void lcdOff(){
//  	lcd.lcdOff();
//  }
void scrInit() {
	
	lcd.InitLCD();
	lcd.clrScr();

	touch.InitTouch();
	touch.setPrecision(PREC_MEDIUM);

	lcd.setFont(BigFont);
	lcd.fillScr(0, 0, 0);
	
	previousMenu = 0;
	choosenMenu = 0;
	
	pumpA.init();
	pumpB.init(pumpA);
	pumpB.init(pumpB);

}
void drawButton(int x1, int y1, int x2, int y2){
	lcd.setColor(0, 0, 255);
	lcd.fillRoundRect (x1, y1, x2, y2); //x_start,y_star, x_end, y_end
	lcd.setColor(255, 255, 255);
	lcd.drawRoundRect (x1, y1, x2, y2);
}
void drawButtonWLabel(int x1, int y1, int x2, int y2, String label, int fontSize = 0) {
	if(fontSize == 0) {
		lcd.setFont(BigFont);
	}
	else {
		lcd.setFont(SmallFont);
	}
	drawButton(x1, y1, x2, y2);
	lcd.setColor(255, 255, 255);
	lcd.setBackColor(0, 0, 255);
	
	int x_label = x1+round((x2-x1)/2)-round((label.length() * lcd.getFontXsize())/2); //font size 16x16
	int y_label = y2-round((y2-y1)/2)-lcd.getFontYsize()/2; //font size 16x16
	lcd.print(label, x_label, y_label);
}
void pressButton(int x1, int y1, int x2, int y2)
{
	lcd.setColor(0, 0, 255);
	lcd.drawRoundRect (x1, y1, x2, y2);
	while (touch.dataAvailable())
	touch.read();
	lcd.setColor(255, 255, 255);
	lcd.drawRoundRect (x1, y1, x2, y2);
}
void drawBar(float temp, String time, String date) {
	lcd.setFont(SmallFont);
	lcd.setColor(0, 0, 0);
	lcd.fillRect (0, 0, 320, 30);
	lcd.setBackColor(0, 0, 0);
	lcd.setColor(255, 255, 255);
	String str_temp = String(temp);
	str_temp = str_temp + " C";
	lcd.print(str_temp, 20, 7);
	lcd.print(time, 120, 7);
	lcd.print(date, 220, 7);
}
void drawPump(int x1, int y1, int x2, int y2, String label) {
	
	int radius = (x2-x1)/2;
	int x = x1 + radius;
	int y = y1 + radius;
	lcd.setFont(BigFont);
	lcd.setColor(0, 0, 255);
	lcd.fillCircle(x, y, radius);
	lcd.setColor(255, 255, 255);
	lcd.drawCircle(x, y, radius);
	
	lcd.setFont(SmallFont);
	int x_label = x-round((label.length() * lcd.getFontXsize())/2); //font size 16x16
	int y_label = y-(lcd.getFontYsize()/2); //font size 16x16
	lcd.setBackColor(0, 0, 255);
	lcd.print(label, x_label, y_label);
	lcd.setBackColor(0, 0, 255);
}
void drawPumpWStatus(int x1, int y1, int x2, int y2, String label, tmElements_t nextDoseTime, int remainingDose) {
	lcd.setFont(SmallFont);
	drawPump(x1, y1, x2, y2, label);
	int radius = x2-x1/2;
	//int y = y1 + radius;
	int x_label = x2 + 5;
	int y_label1 = y1 + (lcd.getFontYsize()/2);
	int y_label2 = y2 - (lcd.getFontYsize());
	lcd.setColor(255, 255, 255);
	lcd.setBackColor(0, 0, 0);
	String str_label1 = "Next dose at " + timeToString(nextDoseTime);
	lcd.print(str_label1, x_label, y_label1);
	String str_label2 = "Remaining dose: " + remainingDose;
	 str_label2 =+ " ml";
	lcd.print(str_label2, x_label, y_label2);
}
void updatePumpStatus(int x1, int y1, int x2, int y2, tmElements_t nextDoseTime, int remainingDose) {
	lcd.setFont(SmallFont);
	int x_label = x2 + 5;
	int y_label1 = y1 + (lcd.getFontYsize()/2);
	int y_label2 = y2 - (lcd.getFontYsize());
	lcd.setColor(255, 255, 255);
	lcd.setBackColor(0, 0, 0);
	String str_label1 = "Next dose at " + timeToString(nextDoseTime);
	lcd.print(str_label1, x_label, y_label1);
	String str_label2 = "Remaining dose: " + remainingDose;
	str_label2 =+ " ml";
	lcd.print(str_label2, x_label, y_label2);
}
void updatePumpPicture(int x1, int y1, int x2, int y2, String label, int colorR, int colorG, int colorB) {
	int radius = (x2-x1)/2;
	int x = x1 + radius;
	int y = y1 + radius;
	lcd.setFont(BigFont);
	lcd.setColor(colorR, colorG, colorB);
	lcd.fillCircle(x, y, radius);
	lcd.setColor(255, 255, 255);
	lcd.drawCircle(x, y, radius);
	
	lcd.setFont(SmallFont);
	int x_label = x-round((label.length() * lcd.getFontXsize())/2); //font size 16x16
	int y_label = y-(lcd.getFontYsize()/2); //font size 16x16
	lcd.setBackColor(0, 255, 0);
	lcd.print(label, x_label, y_label);
	lcd.setBackColor(0, 0, 255);
}
void drawMillis(unsigned long currMilis){
	lcd.setFont(SmallFont);
	String millisMsg = String(currMilis);
	lcd.setBackColor(0,0,0);
	lcd.setColor(255, 255, 255);
	lcd.print(millisMsg, 20, 225);
}
void drawLog(String logMsg){
	lcd.setFont(SmallFont);
	lcd.setBackColor(0,0,0);
	lcd.setColor(255, 255, 255);
	lcd.print(logMsg, 20, 215);
}
//screens drawing, reading, updating
void drawMainScreen(float temp, String time, String date) {
	lcd.clrScr();
	
	currentMenu = MAIN_SCREEN;
	
	drawBar(temp, time, date);
	drawPumpWStatus(10, 40, 40, 70, pumpA.getLabel(), pumpA.getNextDosingDate(), pumpA.getRemainingDose());
	drawPumpWStatus(10, 90, 40, 120, pumpB.getLabel(), pumpB.getNextDosingDate(), pumpB.getRemainingDose());
	drawPumpWStatus(10, 140, 40, 170, pumpC.getLabel(), pumpC.getNextDosingDate(), pumpC.getRemainingDose());
	drawButtonWLabel(240, 60, 310, 110, "Menu");
	drawMillis(millis());
}
void readMainScreen(int x, int y){
	if ((y>=60) && (y<=110))
	{
		if ((x>=240) && (x<=310))  // Button: Menu
		{
			pressButton(240, 60, 310, 110);
			choosenMenu = MAIN_MENU_SCREEN;
		}
	}
}
void drawMainMenuScreen(float temp, String time, String date) {
	lcd.clrScr();
	
	currentMenu = MAIN_MENU_SCREEN;
	
	drawBar(temp, time, date);
	drawButtonWLabel(10, 40, 60, 90, "Pumps", 1);
	drawButtonWLabel(70, 40, 120, 90, "Time", 1);
	drawButtonWLabel(130, 40, 180, 90, "Date", 1);
	drawButtonWLabel(260, 40, 310, 100, "Exit", 1);
	drawMillis(millis());
}
void readMainMenuScreen(int x, int y){
	if ((y>=40) && (y<=90))
	{
		if ((x>=10) && (x<=60))  // Button: Pumps Setup
		{
			pressButton(10, 40, 60, 90);
			choosenMenu = PUMPS_MENU_SCREEN;
		}
		if ((x>=70) && (x<=120))  // Button: Time Setup
		{
			pressButton(70, 40, 120, 90);
			choosenMenu = TIME_SET_MENU_SCREEN;
		}
		if ((x>=130) && (x<=180))  // Button: Date Setup
		{
			pressButton(130, 40, 180, 90);
			choosenMenu = DATE_SET_MENU_SCREEN;
		}
	}
	if ((y>=40) && (y<=100))
	{
		if ((x>=260) && (x<=310))  // Button: Exit menu
		{
			pressButton(260, 40, 310, 100);
			choosenMenu = MAIN_MENU_SCREEN;
		}
	}
}
void drawPumpsMenuScreen(float temp, String time, String date) {
	lcd.clrScr();
	
	currentMenu = PUMPS_MENU_SCREEN;
	
	drawBar(temp, time, date);
	drawButtonWLabel(10, 40, 60, 90, "A Cal",1);
	drawButtonWLabel(70, 40, 120, 90, "A Fill",1);
	drawButtonWLabel(130, 40, 180, 90, "A Dose",1);
	
	drawButtonWLabel(10, 100, 60, 150, "B Cal",1);
	drawButtonWLabel(70, 100, 120, 150, "B Fill",1);
	drawButtonWLabel(130, 100, 180, 150, "B Dose",1);
	drawButtonWLabel(190, 100, 240, 150, "B Delay",1);
	
	drawButtonWLabel(10, 160, 60, 210, "C Cal",1);
	drawButtonWLabel(70, 160, 120, 210, "C Fill",1);
	drawButtonWLabel(130, 160, 180, 210, "C Dose",1);
	drawButtonWLabel(190, 160, 240, 210, "C Delay",1);
	
	drawButtonWLabel(260, 40, 310, 100, "Back",1);
	drawButtonWLabel(260, 140, 310, 210, "Exit",1);
	drawMillis(millis());
}
void readPumpMenuScreen(int x, int y){
	if ((y>=40) && (y<=90)) //Pump A row
	{
		if ((x>=10) && (x<=60))  // Button: PA Calibrate
		{
			pressButton(10, 40, 60, 90);
			choosenMenu = PUMP_PRE_CALIBRATE_A;
		}
		if ((x>=70) && (x<=120))  // Button: PA Fill
		{
			pressButton(70, 40, 120, 90);
			choosenMenu = PUMP_PIPES_FILL_A;
		}
		if ((x>=130) && (x<=180))  // Button: PA Set Dose
		{
			pressButton(130, 40, 180, 90);
			choosenMenu = NUM_KEY_PUMP_SET_DOSE_A;
		}
	}
	if ((y>=100) && (y<=150)) //Pump B row
	{
		if ((x>=10) && (x<=60))  // Button: PB Calibrate
		{
			pressButton(10, 100, 60, 150);
			choosenMenu = PUMP_PRE_CALIBRATE_B;
		}
		if ((x>=70) && (x<=120))  // Button: PB Fill
		{
			pressButton(70, 100, 120, 150);
			choosenMenu = PUMP_PIPES_FILL_B;
		}
		if ((x>=130) && (x<=180))  // Button: PB Set Dose
		{
			pressButton(130, 100, 180, 150);
			choosenMenu = NUM_KEY_PUMP_SET_DOSE_B;
		}
		if ((x>=190) && (x<=240))  // Button: PB Set Delay
		{
			pressButton(190, 100, 240, 150);
			choosenMenu = NUM_KEY_PUMP_SET_DELAY_B;
		}
	}
	/*if ((y>=160) && (y<=210)) //Pump C row INACTIVE DUE TO PUMP UNAVAILABILITY
	{
	if ((x>=10) && (x<=60))  // Button: PC Calibrate
	{
	pressButton(10, 160, 60, 210);
	choosenMenu = PUMP_PRE_CALIBRATE_B;
	}
	if ((x>=70) && (x<=120))  // Button: PC Fill
	{
	pressButton(70, 160, 120, 210);
	choosenMenu = PUMP_PIPES_FILL_C;
	}
	if ((x>=130) && (x<=180))  // Button: PC Set Dose
	{
	pressButton(130, 160, 180, 210);
	choosenMenu = NUM_KEY_PUMP_SET_DOSE_C;
	}
	if ((x>=190) && (x<=240))  // Button: PC Set Delay
	{
	pressButton(190, 160, 240, 210);
	choosenMenu = NUM_KEY_PUMP_SET_DELAY_C;
	}
	}*/
	if ((x>=260) && (x<=310))
	{
		if ((y>=40) && (x<=100))  // Button: BACK TO MAIN menu
		{
			pressButton(260, 40, 310, 100);
			choosenMenu = MAIN_MENU_SCREEN;
		}
		if ((x>=140) && (x<=210))  // Button: Exit menu
		{
			pressButton(260, 140, 310, 210);
			choosenMenu = MAIN_SCREEN;
		}
	}
}
void drawNumKeyScreen(String label){
	lcd.clrScr();
		
	lcd.setFont(SmallFont);
	lcd.setBackColor(0,0,0);
	lcd.setColor(255, 255, 255);
	lcd.print(label, 10, 25);

	drawButtonWLabel(10, 50, 60, 100, "1");
	drawButtonWLabel(70, 50, 120, 100, "2");
	drawButtonWLabel(130, 50, 180, 100, "3");
	drawButtonWLabel(190, 50, 240, 100, "4");
	drawButtonWLabel(250, 50, 300, 100, "5");
	
	drawButtonWLabel(10, 110, 60, 160, "6");
	drawButtonWLabel(70, 110, 120, 160, "7");
	drawButtonWLabel(130, 110, 180, 160, "8");
	drawButtonWLabel(190, 110, 240, 160, "9");
	drawButtonWLabel(250, 110, 300, 160, "0");
	
	drawButtonWLabel(10, 170, 150, 220, "CLEAR");
	drawButtonWLabel(160, 170, 300, 220, "SET");
	lcd.setBackColor(0, 0, 0);
	drawMillis(millis());
	
}
void drawNumKeyResult() {
	lcd.setFont(BigFont);
	lcd.setBackColor(0,0,0);
	lcd.setColor(255, 255, 255);
	lcd.printNumI(numKeyResult, 170, 20);
}
void readNumKeyScreen(int x, int y){
	String result = "";
	if ((y>=50) && (y<=100)) // buttons 1 - 5
	{
		if ((x>=10) && (x<=60))
		{
			pressButton(10, 50, 60, 100);
			result =+ "1";
			drawNumKeyResult();
		}
		if ((x>=70) && (x<=120))
		{
			pressButton(70, 50, 120, 100);
			result =+ "2";
			drawNumKeyResult();
		}
		if ((x>=130) && (x<=180))
		{
			pressButton(130, 50, 180, 100);
			result =+ "3";
			drawNumKeyResult();
		}
		if ((x>=190) && (x<=240))
		{
			pressButton(190, 50, 240, 100);
			result =+ "4";
			drawNumKeyResult();
		}
		if ((x>=10) && (x<=60))
		{
			pressButton(250, 50, 300, 100);
			result =+ "5";
			drawNumKeyResult();
		}
	}
	if ((y>=110) && (y<=160)) // buttons 6 - 0
	{
		if ((x>=10) && (x<=60))
		{
			pressButton(10, 110, 60, 160);
			result =+ "6";
			drawNumKeyResult();
		}
		if ((x>=70) && (x<=120))
		{
			pressButton(70, 110, 120, 160);
			result =+ "7";
			drawNumKeyResult();
		}
		if ((x>=130) && (x<=180))
		{
			pressButton(130, 110, 180, 160);
			result =+ "8";
			drawNumKeyResult();
		}
		if ((x>=190) && (x<=240))
		{
			pressButton(190, 110, 240, 160);
			result =+ "9";
			drawNumKeyResult();
		}
		if ((x>=10) && (x<=60))
		{
			pressButton(250, 110, 300, 160);
			result =+ "0";
			drawNumKeyResult();
		}
	}
	if ((y>=170) && (y<=220)) // buttons clear, set
	{
		if ((x>=10) && (x<=150)) //clear
		{
			pressButton(10, 170, 150, 220);
			result = "";
			drawNumKeyResult();
		}
		if ((x>=160) && (x<=300)) //set
		{
			pressButton(160, 170, 300, 220);
			numKeyResult = result.toInt();
			choosenMenu = previousMenu; //to verify whether it is good idea - menus track can be lost
		}
	}
}
void drawPumpCalibScreen(String pumpLabel, int mode = 0){
	lcd.clrScr();
	
	if(mode == 0) {//pre calibration
		
		lcd.setFont(SmallFont);
		
		String label = "Start calibration (5s) for pump  " + pumpLabel;
		String btnLabel = "Start";
		
		lcd.setBackColor(VGA_TRANSPARENT);
		lcd.setColor(255, 255, 255);
		lcd.print(label, 20, 80);
		
		drawButtonWLabel(80, 110, 240, 160, btnLabel);
	}
	else if(mode == 1)	{ //process results (previous menu = NumKey)
		
		String label = "New calibration will be set for pump " + pumpLabel;
		label = label + " with value " + numKeyResult + " ms/ml";
		String btnLabel = "Confirm";
		
		lcd.setFont(SmallFont);
		
		lcd.setBackColor(VGA_TRANSPARENT);
		lcd.setColor(255, 255, 255);
		lcd.print(label, 20, 80);
		
		drawButtonWLabel(80, 110, 240, 160, btnLabel);
	}

	drawMillis(millis());
	
}
void readPumpCalibScreen(int x, int y){
	if ((y>=110) && (y<=160)) {
		if ((x>=80) && (x<=240)) {
			pressButton(80, 110, 240, 160);
			if(currentMenu == PUMP_PRE_CALIBRATE_A){
				choosenMenu = NUM_KEY_PUMP_CALIBRATE_A;
			}
			else if(currentMenu == PUMP_PRE_CALIBRATE_B) {
				choosenMenu = NUM_KEY_PUMP_CALIBRATE_B;
			}
			else if(currentMenu == PUMP_PRE_CALIBRATE_C) {
				choosenMenu = NUM_KEY_PUMP_CALIBRATE_C;
			}
		}
	}
}
void chooseAction() {
	
	if (touch.dataAvailable())
	{
		touch.read();
		x=touch.getX();
		y=touch.getY();
		if (currentMenu == MAIN_SCREEN)
		chooseActionMain(x, y);
		if(currentMenu == MAIN_MENU_SCREEN)
		chooseActionMainMenu(x, y);
		if(currentMenu == PUMPS_MENU_SCREEN)
		chooseActionPumpMenu(x, y);
		if(currentMenu >= PUMP_PRE_CALIBRATE_A && currentMenu <= PUMP_PRE_CALIBRATE_C)
		chooseActionPumpsCalibration(x, y);
		if(currentMenu == TIME_SET_MENU_SCREEN)
		chooseActionTimeSet(x, y);
		if(currentMenu == DATE_SET_MENU_SCREEN)
		chooseActionDateSet(x, y);
		if(currentMenu >= NUM_KEY_PUMP_CALIBRATE_A && currentMenu <= NUM_KEY_PUMP_CALIBRATE_C)
		chooseActionNumKeyPumpsCalibrate(x, y);
		if(currentMenu >= NUM_KEY_PUMP_SET_DOSE_A && currentMenu <= NUM_KEY_PUMP_SET_DOSE_C)
		chooseActionNumKeyPumpsCalibrate(x, y);
	}
}
void chooseActionMain(int x, int y) {
	//handler for Main screen
	readMainScreen(x, y);
	if(currentMenu != choosenMenu) {
		if (choosenMenu == MAIN_MENU_SCREEN){
			previousMenu = currentMenu;
			drawMainMenuScreen(tMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
			currentMenu = MAIN_MENU_SCREEN;
		}
	}
}
void chooseActionMainMenu(int x, int y) {
	//handler for Main Menu
	readMainMenuScreen(x, y);
	if (currentMenu != choosenMenu) {
		if(currentMenu == PUMPS_MENU_SCREEN) {
			previousMenu = currentMenu;
			drawPumpsMenuScreen(tMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
			currentMenu = PUMPS_MENU_SCREEN;
		}
		else if(choosenMenu == TIME_SET_MENU_SCREEN) {
			previousMenu = currentMenu;
			//TODO: no handling
			
		}
		else if(choosenMenu == DATE_SET_MENU_SCREEN) {
			previousMenu = currentMenu;
			//TODO: no handling
		}
		else if(choosenMenu == MAIN_SCREEN) {
			previousMenu = currentMenu;
			drawMainScreen(tMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
			currentMenu = MAIN_SCREEN;
		}
	}
	
}
void chooseActionPumpMenu(int x, int y) {
	
	//handler for Pumps Menu
	readPumpMenuScreen(x, y);
	//int calibrationResult = 0;
	if(currentMenu != choosenMenu) {
		//pre calibration
		if (choosenMenu == PUMP_PRE_CALIBRATE_A) {
			previousMenu = currentMenu;
			drawPumpCalibScreen(pumpA.getLabel());
			currentMenu = PUMP_PRE_CALIBRATE_A;
		}
		else if (choosenMenu == PUMP_PRE_CALIBRATE_B) {
			previousMenu = currentMenu;
			drawPumpCalibScreen(pumpB.getLabel());
			currentMenu = PUMP_PRE_CALIBRATE_B;
		}
		else if (choosenMenu == PUMP_PRE_CALIBRATE_C) {
			previousMenu = currentMenu;
			drawPumpCalibScreen(pumpC.getLabel());
			currentMenu = PUMP_PRE_CALIBRATE_C;
		}
		
		//pipes fill
		else if (choosenMenu == PUMP_PIPES_FILL_A){
			previousMenu = currentMenu;
			currentMenu = PUMP_PIPES_FILL_A;
			pumpA.fillPipes();
			choosenMenu = PUMPS_MENU_SCREEN;
		}
		else if (choosenMenu == PUMP_PIPES_FILL_B){
			previousMenu = currentMenu;
			currentMenu = PUMP_PIPES_FILL_B;
			pumpB.fillPipes();
			choosenMenu = PUMPS_MENU_SCREEN;
		}
		else if (choosenMenu == PUMP_PIPES_FILL_C){
			previousMenu = currentMenu;
			currentMenu = PUMP_PIPES_FILL_C;
			pumpC.fillPipes();
			choosenMenu = PUMPS_MENU_SCREEN;
		}
		//dosage setup
		else if (choosenMenu == NUM_KEY_PUMP_SET_DOSE_A){
			previousMenu = currentMenu;
			drawNumKeyScreen("Set dose for Pump A (ml)");
			currentMenu = NUM_KEY_PUMP_SET_DOSE_A;
		}
		else if (choosenMenu == NUM_KEY_PUMP_SET_DOSE_B){
			previousMenu = currentMenu;
			drawNumKeyScreen("Set dose for Pump B (ml)");
			currentMenu = NUM_KEY_PUMP_SET_DOSE_B;
		}
		else if (choosenMenu == NUM_KEY_PUMP_SET_DOSE_C){
			previousMenu = currentMenu;
			drawNumKeyScreen("Set dose for Pump C (ml)");
			currentMenu = NUM_KEY_PUMP_SET_DOSE_C;
		}
		//pumps delay setup
		else if (choosenMenu == NUM_KEY_PUMP_SET_DELAY_B){
			previousMenu = currentMenu;
			
		}
		else if (choosenMenu == NUM_KEY_PUMP_SET_DELAY_C){
			previousMenu = currentMenu;
			
		}
		else if (choosenMenu == MAIN_MENU_SCREEN){
			previousMenu = currentMenu;
			drawMainMenuScreen(0, getCurrentTimeStr(), getCurrentDateStr());
			currentMenu = MAIN_MENU_SCREEN;
		}
		else if (choosenMenu == MAIN_SCREEN){
			previousMenu = currentMenu;
			drawMainScreen(0, getCurrentTimeStr(), getCurrentDateStr());
			currentMenu = MAIN_SCREEN;
		}
	}
}
void chooseActionPumpsCalibration(int x, int y) {
	//handler for Pumps calibration
	readPumpCalibScreen(x, y);
	//int calibrationResult = 0;
	if(currentMenu != choosenMenu) {
		//calibration in
		if (choosenMenu == NUM_KEY_PUMP_CALIBRATE_A) {
			previousMenu = currentMenu;
			drawNumKeyScreen("Pump A cal - result in ml*10");
			pumpA.calibrate();
			currentMenu = NUM_KEY_PUMP_CALIBRATE_A;
		}
		else if (choosenMenu == NUM_KEY_PUMP_CALIBRATE_B) {
			previousMenu = currentMenu;
			drawNumKeyScreen("Pump B cal - result in ml*10");
			pumpB.calibrate();
			currentMenu = NUM_KEY_PUMP_CALIBRATE_B;
		}
		else if (choosenMenu == NUM_KEY_PUMP_CALIBRATE_C) {
			previousMenu = currentMenu;
			drawNumKeyScreen("Pump C cal - result in ml*10");
			pumpC.calibrate();
			currentMenu = NUM_KEY_PUMP_CALIBRATE_C;
		}
		//calibration out
		else if (choosenMenu == PUMPS_MENU_SCREEN) {
			previousMenu = currentMenu;
			if(previousMenu == PUMP_PRE_CALIBRATE_A) {
				//TODO:save result
			}
			else if(previousMenu == PUMP_PRE_CALIBRATE_B) {
				//TODO:save result
			}
			else if(previousMenu == PUMP_PRE_CALIBRATE_C) {
				//TODO:save result from calibration
			}
			drawPumpsMenuScreen(0, getCurrentTimeStr(), getCurrentDateStr());
		}
		
	}
	
}
void chooseActionTimeSet(int x, int y) {
	
	//TODO: handler
}
void chooseActionDateSet(int x, int y) {

	//TODO: handler
}
void chooseActionNumKeyPumpsCalibrate(int x, int y) {
	//handler for numKeys (calibrate)
	readNumKeyScreen(x, y);
	//int calibrationResult = 0;
	if(currentMenu != choosenMenu) {
		if (choosenMenu == PUMP_PRE_CALIBRATE_A){
			previousMenu = currentMenu;
			drawPumpCalibScreen(pumpA.getLabel(), 1);
			pumpA.setCalibration(numKeyResult);
			drawLog("pumpA.setCalibration " + numKeyResult);
			currentMenu = PUMP_PRE_CALIBRATE_A;
		}
		else if (choosenMenu == PUMP_PRE_CALIBRATE_B){
			previousMenu = currentMenu;
			drawPumpCalibScreen(pumpB.getLabel(), 1);
			pumpB.setCalibration(numKeyResult);
			drawLog("pumpB.setCalibration " + numKeyResult);
			currentMenu = PUMP_PRE_CALIBRATE_B;
		}
		else if (choosenMenu == PUMP_PRE_CALIBRATE_C){
			previousMenu = currentMenu;
			drawPumpCalibScreen(pumpC.getLabel(), 1);
			pumpC.setCalibration(numKeyResult);
			drawLog("pumpC.setCalibration " + numKeyResult);
			currentMenu = PUMP_PRE_CALIBRATE_C;
		}
		else {
			drawLog("calibration - no setup " + numKeyResult);
		}
		numKeyResult = 0;
	}
	
}
void chooseActionNumKeyPumpsSetup(int x, int y) {
	//handler for numKeys (calibrate)
	readNumKeyScreen(x, y);
	//int calibrationResult = 0;
	if(currentMenu != choosenMenu) {
		if (choosenMenu == PUMPS_MENU_SCREEN){
					
			previousMenu = currentMenu;
			if(currentMenu == NUM_KEY_PUMP_SET_DOSE_A && numKeyResult > 0) {
				pumpA.setDosage(numKeyResult);
				drawLog("pumpA.setDosage " + numKeyResult);
			}
			else if(currentMenu == NUM_KEY_PUMP_SET_DOSE_B && numKeyResult > 0) {
				pumpB.setDosage(numKeyResult);
				drawLog("pumpC.setDosage " + numKeyResult);
			}
			else if(currentMenu == NUM_KEY_PUMP_SET_DOSE_C && numKeyResult > 0) {
				pumpB.setDosage(numKeyResult);
				drawLog("pumpC.setDosage " + numKeyResult);
			}
			else {
				drawLog("dose setup - no setup " + numKeyResult);
			}
			numKeyResult = 0;
			drawPumpsMenuScreen(tMeter.getTemperature(), getCurrentTimeStr(), getCurrentDateStr());
			currentMenu = PUMPS_MENU_SCREEN;
		}
	}
}


