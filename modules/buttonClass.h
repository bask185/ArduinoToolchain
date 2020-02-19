#ifndef button_h
#define	button_h

enum states {
	RISING,
	FALLING,
	ON,
	OFF };

class Debounce {
public:
	Debounce(unsigned char _pin);
	readInput();
	debounceInputs();

private:
	unsigned char state;
	unsigned char pin; };
	
#endif