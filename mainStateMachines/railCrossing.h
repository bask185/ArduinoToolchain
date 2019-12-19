enum railCrossingStates {
	railCrossingIDLE,
	closeCrossing,
	openCrossing,
	waitCrossing };

static void nextState(unsigned char, unsigned char);
extern void railCrossing(void); 
extern void railCrossingStop(void);
extern void railCrossingSetState(unsigned char);
extern unsigned char railCrossingGetState(void);
