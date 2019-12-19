enum trainControllStates {
	trainControllIDLE,
	accelerateLeft,
	deccelerateLeft,
	accelerateRight,
	deccelerateRight };

static void nextState(unsigned char, unsigned char);
extern void trainControll(void); 
extern void trainControllStop(void);
extern void trainControllSetState(unsigned char);
extern unsigned char trainControllGetState(void);
