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


print(timers)
print(timeBases)

###
with open("timers.cpp", "w") as scheduler:  #scheduler.c 
    scheduler.write("#include <Arduino.h>\n")
    scheduler.write('#include "scheduler.h"\n\n')
    scheduler.write("extern void initTimers() {")
    scheduler.write("""
	cli();//stop interrupts
    //set timer1 interrupt at 1kHz
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set timer count for 1khz increments
    OCR1A = 1999;// = (16*10^6) / (1000*8) - 1
    //had to use 16 bit timer1 for this bc 1999>255, but could switch to timers 0 or 2 with larger prescaler
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS11 bit for 8 prescaler
    TCCR1B |= (1 << CS11);  
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    sei();//allow interrupts

""")

    for timer in timers:
        scheduler.write("unsigned char " + timer + ";\n")

    scheduler.write("""
// Don't complain about the indentations. This code is generated for you and you shouldn't be looking at this part.
ISR(TIMER1_COMPA_vect) {
static unsigned char _1ms, _10ms, _100ms, _1000ms;

_1ms += 1; 
	// 1ms timers
""")
    for index, timer in enumerate(timers):
        if int(timeBases[index]) == 1:
            scheduler.write('\tif(' + timer + ') ' + timer + '--;\n')

    scheduler.write("""
if(!(_1ms % 10)) { _1ms = 0; _10ms += 1;
	// 10ms timers
""")
    for index, timer in enumerate(timers):
        if int(timeBases[index]) == 10:
            scheduler.write('\tif(' + timer + ') ' + timer + '--;\n')

    scheduler.write("""
if(!(_10ms % 10)) { _10ms = 0; _100ms += 1;
	// 100ms timers
""")
    for index, timer in enumerate(timers):
        if int(timeBases[index]) == 100:
            scheduler.write('\tif(' + timer + ') ' + timer + '--;\n')

    scheduler.write("""
if(!(_100ms % 10)) { _100ms = 0; _1000ms += 1;
	// 1000ms timers
""")
    for index, timer in enumerate(timers):
        if int(timeBases[index]) == 1000:
            scheduler.write('\tif(' + timer + ') ' + timer + '--;\n')

    scheduler.write("""
if(!(_1000ms % 60)) { _1000ms = 0;
	// 1 minute timers
""")
    for index, timer in enumerate(timers):
        if int(timeBases[index]) == 60000:
            scheduler.write('\tif(' + timer + ') ' + timer + '--;\n')

    scheduler.write("`\n}\n}\n}\n}\n}")
    scheduler.close()


with open("timers.h", "w") as scheduler:  #scheduler.h
    scheduler.write("extern void initTimers();\n\n")
    for timer in timers:
        scheduler.write("extern unsigned char " + timer + ";\n")
    scheduler.close()
