enum weatherStatesStates {
	weatherStatesIDLE,
	morning,
	afternoon,
	evening,
	night,
	rain,
	thunder };

static void nextState(unsigned char, unsigned char);
extern void weatherStates(void); 
extern void weatherStatesStop(void);
extern void weatherStatesSetState(unsigned char);
extern unsigned char weatherStatesGetState(void);
