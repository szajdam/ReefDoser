


bool getFileDate(const char *str) {
	char Month[12];
	int Day, Year;
	uint8_t monthIndex;

	if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3)
	return false;
	for (monthIndex = 0; monthIndex < 12; monthIndex++) {
		if (strcmp(Month, monthName[monthIndex]) == 0)
		break;
	}
	if (monthIndex >= 12)
	return false;
	currentTime.Day = Day;
	currentTime.Month = monthIndex + 1;
	currentTime.Year = CalendarYrToTm(Year);
	return true;
}

bool getFileTime(const char *str) {
	int Hour, Min, Sec;

	if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3)
	return false;
	currentTime.Hour = Hour;
	currentTime.Minute = Min;
	currentTime.Second = Sec;
	return true;
}

void readTime() {
	if (!RTC.read(currentTime)) {
		if (RTC.chipPresent()) {
			drawLog("The DS1307 is stopped.  Please set Time");
			} else {
			drawLog("DS1307 read error!  Please check the circuitry.");
		}
	}
}

void setRTCTimeFromFile() {
	bool parse = false;
	bool config = false;

	// get the date and time the compiler was run
	if (getFileDate(__DATE__) && getFileTime(__TIME__)) {
		parse = true;
		// and configure the RTC with this info
		if (RTC.write(currentTime)) {
			config = true;
		}
	}

	if (parse && config) {
		//log("DS1307 configured Time=");
		//log(__TIME__);
		//log(", Date=");
		//logln(__DATE__);
		} else if (parse) {
		//logln("DS1307 Communication Error :-{");
		//logln("Please check your circuitry");
		} else {
		//log("Could not parse info from the compiler, Time=\"");
		//log(__TIME__);
		//log("\", Date=\"");
		//log(__DATE__);
		//logln("\"");

	}
}

String getCurrentTimeStr() {
	timeToString(currentTime);
}

String timeToString(tmElements_t time) {
	String timeStr = to2Digits(String((int) currentTime.Hour));
	timeStr = timeStr + ":";
	timeStr = timeStr +  to2Digits(String((int) currentTime.Minute));
	timeStr = timeStr + ":";
	timeStr = timeStr +  to2Digits(String((int) currentTime.Second));
	return timeStr;

}


String getCurrentDateStr() {
	dateToString(currentTime);
}
String dateToString(tmElements_t date) {
		String dateStr = to2Digits(String((int) tmYearToCalendar(currentTime.Year)));
		dateStr = dateStr + "-";
		dateStr = dateStr +  to2Digits(String((int) currentTime.Month));
		dateStr = dateStr + "-";
		dateStr = dateStr +  to2Digits(String((int) currentTime.Day));
		return dateStr;
}


String to2Digits(String number) {
	if (number.length() < 2) {
		return "0" + number;
	}
	return String(number);
}


