enum menuStates {
	menuIDLE,
	mainMenu,
	auto_,
	test,
	settings,
	automatic,
	inputs,
	outputs,
	testLoadSystems,
	testIndexTable,
	testPunchUnits,
	powerOff };

static void nextState(unsigned char, unsigned char);
extern void menu(void); 
extern void menuStop(void);
extern void menuSetState(unsigned char);
extern unsigned char menuGetState(void);
