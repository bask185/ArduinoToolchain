# import sys
# import xml
# import os

timers = []
timeBases = []

with open("timers.tab" , "r") as f:
	lines = f.readlines()
	for line in lines:
		local = line.split('\t')
		timers.append(local[0])
		timeBases.append(local[1][:-1]) # without the the \n char


#print(timers)
#print(timeBases)

###
with open("src/basics/timers.cpp", "w") as scheduler:  #scheduler.c 
    scheduler.write("#include <Arduino.h>\n")
    scheduler.write('#include "timers.h"\n\n')
    scheduler.write("extern void initTimers() {")
    scheduler.write("""
	TCCR2B = 0;// same for TCCR2B
	TCNT2  = 0;//initialize counter value to 0
	// set compare match register for 8khz increments
	OCR2A = 249;// = (16*10^6) / (1000*64) - 1 (must be <256)
	// turn on CTC mode
	TCCR2A |= (1 << WGM21);
	// Set CS21and and CS20 bit for 64 prescaler
	TCCR2B |= (1 << CS20);
	TCCR2B |= (1 << CS21); 
	TCCR2B &= ~(1 << CS22); 
	// enable timer compare interrupt
	TIMSK2 |= (1 << OCIE2A); }

""")

    for timer in timers:
        scheduler.write("volatile unsigned char " + timer + ";\n")

    scheduler.write("""
// Don't complain about the indentations. This code is generated for you and you shouldn't be looking at this part.
ISR(TIMER2_COMPA_vect) {
static unsigned char _1ms, _10ms, _100ms;

// 1ms timers
_1ms += 1;

""")
    for index, timer in enumerate(timers):
        if int(timeBases[index]) == 1:
            scheduler.write('\tif(' + timer + ') ' + timer + '--;\n')

    scheduler.write("""


// 10ms timers
if(_1ms == 10) { _1ms = 0; _10ms += 1;\n\n""")
    for index, timer in enumerate(timers):
        if int(timeBases[index]) == 10:
            scheduler.write('\tif(' + timer + ') ' + timer + '--;\n')

    scheduler.write("""


// 100ms timers
if(_10ms == 10) { _10ms = 0; _100ms += 1;\n\n""")
    for index, timer in enumerate(timers):
        if int(timeBases[index]) == 100:
            scheduler.write('\tif(' + timer + ') ' + timer + '--;\n')

    scheduler.write("""


//1000ms timers
if(_100ms == 10) { _100ms = 0;\n\n""")
    for index, timer in enumerate(timers):
        if int(timeBases[index]) == 1000:
            scheduler.write('\tif(' + timer + ') ' + timer + '--;\n')

    scheduler.write("\n\n\n}\n}\n}\n}\n\n")

    scheduler.write("""
extern void repeat(byte *timer, byte interval, void (*function)()) {
	if(*timer == 0) {
		*timer = interval;
		function();
	}
}""")
    scheduler.close()


with open("src/basics/timers.h", "w") as scheduler:  #scheduler.h
    scheduler.write("extern void repeat(byte* ,byte, void (*)());\n")
    scheduler.write("extern void initTimers();\n\n")
    for timer in timers:
        scheduler.write("extern volatile unsigned char " + timer + ";\n")
    scheduler.close()
