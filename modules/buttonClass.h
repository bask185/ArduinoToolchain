#ifndef button_h
#define	button_h

enum states {
	RISING,
	FALLING,
	PRESSED,
	RELEASED };

class Button {
public:
	Button(unsigned char _pin);
	readButton();
	updateButton();

private:
	unsigned char state;
	unsigned char pin; };
	
#endif