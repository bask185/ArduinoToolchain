#include <Arduino.h>

#ifndef button_h
#define	button_h

//#define 
#define ON 9
#define OFF 10

class Debounce {
public:
	Debounce(unsigned char _pin);
	unsigned char readInput();
	void debounceInputs();

private:
	unsigned char state;
	unsigned char pin; 
};
	
#endif