enum buttonTestStates {
	buttonTestIDLE,
	a,
	b,
	c,
	d };

static void nextState(unsigned char, unsigned char);
extern void buttonTest(void); 
extern void buttonTestStop(void);
extern void buttonTestSetState(unsigned char);
extern unsigned char buttonTestGetState(void);
