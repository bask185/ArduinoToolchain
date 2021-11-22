# State Machines & Automated Project Assembly

The purpose of this github page is to provide you with the tools and knowledge to set up a 99% compile-able arduino folder complete with state machines skeletons, accompanying timers and other supplemental modules. Simple syntax makes it use-able for both beginning as well as experience programmers alike.

The main aims are:
- to help you setting up new projects with lightning speed, including build scripts, git repository and VS code tasks.
- to help you preventing bugs by generating as much source code as possible for you
- to offer you a partial generated powerfull state-machine structure to assist you with coding complex tasks.


### List of all features :
---
- you can set up new project folder with the press on a button.
- all files are properly organized as is the code
- IO and their tri-states are managed in a single file. You can never forget to type a pinMode instruction
- IO files come with built in mcp23017 io extender functionality
- Files and code structure are in place for round robin tasks
- Fully functional state machine skeletons are generated, using yEd graphical state diagrams as input
- The assembleProject script has a built in feature to pick modules (libraries) from the module folder and copy them to your new project folder
- Filling in of State machines and accopanying functions makes it easy to code your tasks.
- State machines are kept modular
- Code generation, project assembly, simple syntax and simple state machine operation help a great deal with preventing bugs.
- build and compile scripts are included for usage with arduino-cli.exe
- VS code tasks are included for compiling and uploading
- Automated git repository initialization
- date and time stamp are generated and hardcoded in the project before every compilation. Whenever your arduino runs it's program. The date/time of last compilation is sent to the serial monitor
- built in debug functionality within state machines
- Source code is compliant with Arduino's Hardware Abstraction Layers.

Further documentation is provided on this github page. You can find information about the software timers, round robin tasks, module system and the state machines.


## Dependencies
---
You need python in order to run the python scripts and yEd to create simple state diagrams. One such a state diagram is included in this repository so you can test the scripts before you instal yEd.

## Usage
---
Once you are set up, the basis usage is very simple. There are just two things you have to do to create a new project folder.
- Either create a state diagram or copy the example demo.graphml to the 'stateMachines' folder.
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


## The State Machines
---
The state machines as they are implemented have a painfully clear and easy to use syntax. I'll show an example of a generated state machine with 2 states:
```c
// STATE FUNCTIONS
stateFunction( monitorSw1 )
{
    if( sm.entryState() )
	{
        
    }
    if( sm.onState() )
	{
        sm.exit() ;
    }
    if( sm.exitState() )
	{

        return true; 
    }

	sm.endState() ;
}


stateFunction( monitorSw2 )
{
    if( sm.entryState() ) 
	{
        
    }
    if( sm.onState() ) 
	{

        sm.exit() ;
    }
    if( sm.exitState() )
	{

    }

	sm.endState() ;
}

// STATE MACHINE
extern uint8_t readTrack1()
{
    STATE_MACHINE_BEGIN

    State( monitorSw1 ) {
        nextState( monitorSw2, 0 ); }

    State( monitorSw2 ) {
        nextState( monitorSw1, 0 ); }

    STATE_MACHINE_END
}
```

This state machine is actually a switch-case disguised by macros. A `State()` actually contains a case label. `State(monitorSw1Sw2)` performs a function call to `stateFunction(monitorSw1Sw2)`. This stateFunction returns true once the exitState is executed. When this happens, the function `nextState()` is called to select a new state with or without a timed delay. The state names themselfes are an enum which lie in the header file. New states should be added there.

The state machine in the bottom need no filling in. You could adjust the 0 to a value of choise to achief a delay between following states in milliseonds. This particular state machine is called 'readTrack1' this used to be the name of the yEd state diagram. The timer for this state machine is therefor called readTrack1T. This one is also used by the nextState function to achief the delay. This is how the basic functionality of a state machine functions.

It may be that one of your states can transition to more than one other state. In that case the state machine also need filling in because the following code would be generated.
```c
State( X ) {
    nextState( Y, 0 ) ;
    nextState( Z, 0 ) ; }
```
This is not complete. We need to add something so the state machine picks one of the two following states.
```c
State( X ) {
    if(/* condition */) nextState( Y, 0 ) ;
    else                nextState( Z, 0 ) ; }
```
The reason why a certain state is to be picked must be filled in. This reason why I call the _flow condition_

I will show an example of a filled in state which waits on a certain input and has a time-out. I will make use of an entry and exit states to print messages.
```c
stateFunction( monitorSw1 )
{
    if( sm.entryState() )
    {
        Serial.println( "entering state monitorSw1" ) ;
        sm.setTimeout() ;
    }
    if( sm.onState() )
    {
        if( !mcpRead( sensor ) )  // if sensor is made on time
		{
            sm.exit() ;
        }
        if( sm.timeout() )    // if timeout occurs
		{
            error = true ;    // set some error flag
            sm.exit() ; 
        }
    }
    if( sm.exitState() )
    {
        Serial.println( "leaving state monitorSw1" ) ;
        if(error) { Serial.println( "TIME OUT OCCURED" ) ; }
	}

	return sm.endState() ;
}
```
This state will run the exit state if either the sensor is made or a timeout occurs. If the timeout occurs the `error` flag is set.

We could use this same `error` flag in the state machine like this:
```c
State( monitorSw1 )
{
    if( error ) nextState( abort, 0 ) ;
    else        nextState( monitorSw2, 0 ) ;
}
```

When you start using this software, you will see that the state machine has a 101 relation with your state diagram. 
- For every amount of  spheres there is an equal amount of states
- For every amount of outgoing arrows there is an equal amount of calls to `nextState` 
- For every amount of arrows pointing to state `X` there is an equal amount of calls to `nextState(X,0)` 

Other features are:

- clear syntax of state functions makes it easy to fill in stateFunctions. 
- This easiness helps to prevent bugs.
- Everything is kept as modular as possible. It is relative easy to add new states, remove states or change the flow of the state machine


## Brief explanation of IO
---
IO are managed in `io.tab`. The script 'updateIO.py' is needed to generate the source files. This happens before compiling.

I use the .tab file and the script so I can manage all IO in a single place. Manually changing IO also means editing two places in two files. Now you just have to alter one thing in one file.

In `io.tab` you must fill in a pin number followed by: tab, name/description, tab, iodir (OUTPUT, INPUT or INPUT_PULLUP). 
```
7    sensorLeft    INPUT_PULLUP
```
The script will generate the source files. #defines are made for all descriptions in the header file and the source file will contain the ```initIO()``` function. This function sets the pinModes for us. We can never forget  to use a pinMode instruction anymore. ```initIO()``` is called from ```void setup()```. As this is generated for us, we cannot forget to run the init function either.

These IO files have integrated support for the MCP23017 I2C IO extender. Making IO for these devices can also be done in the same io.tab. There can be as much as 8 MCP23017 devices. Take note that every slave has just 16 GPIO. So use 0-15 for every IO
```c
7    sensorLeft    INPUT_PULLUP

MCP1
15    sensorMidde    INPUT

MCP2
4    led4    OUTPUT
// ETC
```
```initIO()``` does the configuring of these devices for us. To control the IO you can use two functions called ```mcpWrite(io, state);``` and ```mcpRead(io);```. They work the exact same as ```digitalWrite();``` and ``` digitalRead()``` The beauty is that we do not have to remember which IO is on which mcp device and on which port. These functions calculate the correct slave, port and pin number. Writing to a pin does not affect other pins. 

The only condition is that the hardware adresses of the slaves must be in incrementing order. That means that MCP1 must have address 0x20, MCP2 must have address 0x21, etc 
