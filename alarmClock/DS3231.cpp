enum {
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
	SUNDAY }

#define WINTER 0
#define SUMMER 1



DS1307::void setTime(unsigned char hour, unsigned char minute) {
	Wire.beginTransmission(address);
	Wire.write(minuteBank);
	Wire.write(minute);
	Wire.write(hour);
	Wire.endTransmission(); }

DS1307::void setAlarmTime(unsigned char hour, unsigned char minute) {
	alarmHour = hour;
	alarmMinute = minute; }

DS1307::void getTime(struct time* localTime) {
	Wire.beginTransmission(address);
	Wire.write(secondBank);
	Wire.endTransmission();
	Wire.requestFrom(address, 7); // request 2 bytes
	second				= bcdToDecimal(Wire.read()); // private
	localTime -> minute = bcdToDecimal(Wire.read()); // public
	localTime -> hour	= bcdToDecimal(Wire.read());
	localTime -> day	= bcdToDecimal(Wire.read());
	localTime -> date	= bcdToDecimal(Wire.read()); 
	localTime -> month	= bcdToDecimal(Wire.read());
	localTime -> year	= bcdToDecimal(Wire.read()); }

DS1307::void getAlarmTime(struct time* localTime) {
	localTime -> minute	= alarmHour;
	localTime -> hour	= alarmMinute; }

DS1307::void handleSummerWinterTime() {
	if( 
	MONTH == 10			&& // october
	DAY == SUNDAY		&&
	DAY / 24			&& // last SUNDAY
	season == SUMMER	&&
	HOUR == 3			&&
	MINUTE == 0) {

		season = WINTER;
		Wire.beginTransmission(address);
		Wire.write(hourBank);
		Wire.endTransmission();
		Wire.requestFrom(address,1);	// stashes 1 byte in i2c buffer
		Wire.beginTransmission(address);
		Wire.write(hourBank);
		Wire.write(Wire.read() - 1);	// pops the buffered byte and substracts 1
		Wire.endTransmission(); }

	else if(
	MONTH == 3			&& // march
	DAY == SUNDAY		&&
	DAY / 24			&& // last SUNDAY
	season == WINTER	&&
	HOUR == 2			&&
	MINUTE == 0) {
		
		season = SUMMER;
		Wire.beginTransmission(address);
		Wire.write(hourBank);
		Wire.endTransmission();
		Wire.requestFrom(address,1);	// stashes 1 byte in i2c buffer
		Wire.beginTransmission(address);
		Wire.write(hourBank);
		Wire.write(Wire.read() + 1);	// pops the buffered byte and adds 1
		Wire.endTransmission(); } }// set time 1 hour forward

DS1307::bool checkAlarm() {
	if(	
	alarmEnabled			&&
	hour == alarmHour		&&
	minute == alarmMinute	&&
	second == 0) {
		return true; }
	else {
		return false; } }

int bcdToDecimal(unsigned char x) {
	return x - 6 * (x >> 4); }
