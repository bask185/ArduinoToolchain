# State Machines & project assembly
The purpose of this github page is to provide you with the tools and explanations to set up a 99% compile-able arduino folder complete with state machines skeletons, software timers and other supplemental modules. Simple syntax makes it use-able for both beginning as well as experience programmers alike.

The syntax is kept as clear as possible as is the state machine structure. Once you set up your project, the only thing you have to do is to fill in the partial filled in state functions and a few bits of the state machine.

---
The main aims of this project are to help you with:
- preventing you from making bugs for as much as possible.
- coding the most complex tasks
- keeping your code and project organized

---
The features are:
- you can set up new project folder with the press on a button.
- all files are properly organized as is the code
- IO and their tri-states are managed in a single file. You can never forget to type a pinMode instruction
- IO files come with built in mcp23017 io extender functionality
- all software Timers are 8 bits, have a customizable time-base and are managed in a single file.
- Files and code structure are in place for round robin tasks
- Fully functional state machine skeletons are generated for you using an yEd state diagram
- functionality for nested state machines is built in.
- The assembleProject script has a built in feature to pick modules (libraries) from the module folder and copy them to your new project folder
- The assembleProject script can be easily altered to handle your own customized features (automatically initializing and calling one of your modules can be done)
- All coding you need to do exists out of simply filling in the generated state functions and round robin tasks
- State machines are kept modular
- Code generation, project assembly, simple syntax and simple state machine operation help a great deal with preventing bugs.


Further documentation is provided on this github page. You can find information about the software timers, round robin tasks, module system and the state machines.

# Disclaimer

Though the project assembly is designed arround state-machine, you can also generate projects without them. You will still have a round robin tasks, IO generation, mcp23017 support and timer generation available to you.

The mcp23017 functions are not yet tested, but I expect them to work.

The debounceClass module itself is finished but also untested nor implemented. And it does not have much documentation. It is OO style programmed. With an `object.readInput()` method any Arduino input can be debounced. For every object, the method `object.debounceInput()` is to be called every 20ms or so.
In the future I will add an automated implementation for this module. The `readInput()` method returns "ON, OFF, FALLING or RISING". In other words: is still work in progress.


Also I really suck at programming in Python. I've been told that the parsing of the .graphml is really crappy. But the scripts still do their jobs.

# Dependencies
You need python in order to run the python scripts and yEd to create simple state diagrams. One such a state diagram is included in this repository so you can test the scripts before you instal yEd.

# Setup
In the cloned or downloaded repository you need to create two sub-folders called 'mainStateMachines' and 'nestedStateMachines'. This has to be done just once.

# Usage
Once you are set up, the basis usage is very simple. There are just 3 things you have to do to create a new project folder.
- Either create a state diagram or copy the example from the yEd_stateDiagram directory to the 'mainStateMachines' folder.
- Optionally, fill in your IO definitions in io.tab conform the example. This can also be done after the project is assembled in the new folder.
- double click and run 'assembleProject.py' and enter a name for a project. This script will also ask you if you want to include modules but you can just type 'done' and hit Enter.

The script will generate files, copy files and move files to your new project folder.

Your new project is now assembled and lies in the same folder as the state-machine-script folder. It is not yet 100% compile-able. 

Every state machine need to have their 'beginState' defined. By default the #define is commented out. If you try to compile the compile error will show you precisely what you must do.
```c
//#define beginState
#ifndef beginState
#error beginState not yet defined
#endif
```

Once the 'beginState' is defined your project is compile-able. From this point on you only have to fill in the stateFunctions and a few flow conditions in the state machine. If you have round robin tasks you can fill these in in roundRobinTasks.cpp. The structure is already in place and the syntax is kept simple.

---
# The new Folder
An assembled project folder may look like this:
![project folder](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/project_folder.png)

This example has 2 state-machines included. The files marked with the red arrows are to be programmed by us.

The .ino file is already filled in and looks like:
```c
#include "src/basics/timers.h"
#include "src/basics/io.h"
#include "roundRobinTasks.h"
#include "buttonTest.h"

void setup() {
	initTimers();
	initIO();
	Serial.begin(115200);
	buttonTestInit();
}

void loop() {
	processRoundRobinTasks();

	buttonTest();
}
```
Note that the buttonTest() state-machine is a main state-machine. The other one, `weatherStates`, is a nested state-machine and is therefor not called from `void loop()`.

# Brief explanation of software timers and IO
Timers are managed in the file `timers.tab` and IO are managed in `io.tab`. The scripts 'updateTimers.py' and 'updateIO.py' are needed to generate the source files. When a project folder is assembled, these scripts wil be run by the main assembly script.

I use the .tab files and the scripts so I can manage all timers in one place and all IO in one place. Manually adding a timer means editing three places in two files. Manually changing IO also means editing two places in two files. Now you just have to alter one thing in one file and double click on a .py script.

## Timers
In timers.tab you can add new timers followed by a timebase interval of 1, 10, 100 or 1000ms. There must be 1 tab between the words (untill I write a better script). Every main state machine has automatically one timer assigned @ a time base of 10ms. If you have changed timers.tab you must run the script 'updateTimers.py' before the changes take effect. This script (re)generates timers.cpp and timers.h and puts them in the src/basics sub directory. You can do this as often as needed. It is never needed to manipulate timers.cpp and timers.h yourself.
``` 
timerT 100
```

All timers are 8 bit large and are of decrementing order. They count down to zero. You can simply use the `=` operator to stuff a value in it, and use `if` to check if a timer reaches 0.

```c
if(!timerT) {
	timerT = someInterval;
	// do something every someInterval
}
```
These timers have proven to be extremely usefull and flexible in combination with the state machine structure. 

In addition I have made a simple function called ```repeat();```
This function calls any given function @ interval time using a timer of choise. But you don't have to use it.

```c
repeat(&timer1T, 100, foo); // this function needs to use a pointer 
repeat(&timer2T, 200, bar); // unfortunately
```



## IO
The IO work in the same manner. In `io.tab` you must fill in a pin number followed by: tab, name/description, tab, iodir (OUTPUT, INPUT or INPUT_PULLUP). 
```
7	sensorLeft	INPUT_PULLUP
```
The script will generate the source files. #defines are made for all descriptions in the header file and the source file will contain the ```initIO()``` function. This function sets the pinModes for us. We can never forget  to use a pinMode instruction anymore. ```initIO()``` is called from ```void setup()```. As this is generated for us, we cannot forget to run the init function either.

These IO files have integrated support for the MCP23017 I2C IO extender. Making IO for these devices can also be done in the same io.tab. There can be as much as 8 MCP23017 devices. Take note that every slave has just 16 GPIO. So use 0-15 for every IO
```c
7	sensorLeft	INPUT_PULLUP

MCP1
15	sensorMidde	INPUT

MCP2
4	led4	OUTPUT
// ETC
```
```initIO()``` does the configuring of these devices for us. To control the IO you can use two functions called ```mcpWrite(io, state);``` and ```mcpRead(io);```. They work the exact same as ```digitalWrite();``` and ``` digitalRead()``` The beauty is that we do not have to remember which IO is on which mcp device and on which port. These functions calculate the correct slave, port and pin number. Writing to a pin does not affect other pins. 

The only condition is that the hardware adresses of the slaves must be in incrementing order. 

# The state machines
The state machines as they are implemented have a painfully clear and easy to use syntax. I'll show an example of a generated state machine with 2 states:
```c
// STATE FUNCTIONS
stateFunction(monitorSw1) {
	entryState {
		
	}
	onState {

		exitFlag = true; 
	}
	exitState {

		return true; 
	}
}


stateFunction(monitorSw2) {
	entryState {
		
	}
	onState {

		exitFlag = true;
	}
	exitState {

		return true; 
	} 
}

// STATE MACHINE
extern bool readTrack1(void) {
	STATE_MACHINE_BEGIN

	State(monitorSw1) {
		nextState(monitorSw2, 0); 
	}

	State(monitorSw2) {
		nextState(monitorSw1, 0);
	}

	STATE_MACHINE_END
}
```
You see a lot of macros. It is not needed to know how everything works in detail or how the macros look like. I am convinced that you should be able to 'guess' that code behind the 'entryState' macros is run only once and that you need to set the exitFlag in order to run the exitState.

What you must know is this. The state machine is actually a switch-case disguised by macros. A `State()` is actually a case label. `State(monitorSw1Sw2)` performs a function call to `stateFunction(monitorSw1Sw2)`. This stateFunction returns true once the exitState is executed. When this happens, the function `nextState()` is called to select a new state with or without a timed delay. The state names themselfes are an enum which lie in the header file. New states must be added there.

The state machine in the bottom need no filling in. You could adjust the 0 to a value lower than <255 to achief a delay between following states. This particular state machine is called 'readTrack1' this used to be the name of the yEd state diagram. The timer for this state machine is therefor called readTrack1T. This one is also used by the nextState function to achief the delay. This is how the basic functionality of a state machine functions.

It may be that one of your states can transition to more than one other state. In that case the state machine also need filling in because the following code would be generated.
```c
State(X) {
	nextState(Y, 0);
	nextState(Z, 0);
}
```
This is not complete. We need to add something so the state machine picks one of the two following states.
```c
State(X) {
	if(/* condition */) nextState(Y, 0);
	else                nextState(Z, 0);
}
```
The reason why a certain state is to be picked must be filled in. This reason why I call the _flow condition_

I will show an example of a filled in state which waits on a certain input and has a time-out. I will make use of an entry message and exit message:
```c
stateFunction(monitorSw1) {
	entryState {
		Serial.println("entering state monitorSw1");
		readTrack1 = 150; // set timeout time
	}
	onState {
		if(!mcpRead(sensor)) { // if sensor is made on time
			exitFlag = true;
		}
		if(!readTrack1T) {	// if timeout occurs
			error = true;	// set some error flag
			exitFlag = true; 
		}
	}
	exitState {
		Serial.println("leaving state monitorSw1");
		if(error) { Serial.println("TIME OUT OCCURED"); }
		return true; } }
```
This state will run the exit state if either the sensor is made or a timeout occurs. If the timeout occurs the `error` flag is set.

We could use this same `error` flag in the state machine like this:
```c
State(monitorSw1) {
	if(error) nextState(abort, 0);
	else      nextState(monitorSw2, 0);
}
```
When you start using this software, you will see that the state machine has a 101 relation with your state diagram. 
- For every amount of  spheres there is an equal amount of states
- For every amount of outgoing arrows there is an equal amount of calls to `nextState` 
- For every amount of arrows pointing to state `X` there is an equal amount of calls to `nextState(X,0)` 

This is one of the most powefull features this code structure has. A 101 relation with a state diagram. Other features are:

- clear syntax of state functions makes it easy to fill in stateFunctions. 
- This easiness helps to prevent bugs.
- software timers are extremely simple. They have very little overhead and the operation is simple.
- not that much overhead in general
- Everything is kept as modular as possible. It is relative easy to add new states, remove states or change the flow of the state machine
- easy to use nested state machines fromout an other state machine.
  
