# State Machine Scripts & project assembly
The purpose of this github page is to provide you with the tools and explanations to set up a fully compile-able arduino folder complete with state machines structures, software timers, debounced button reading and other supplemental modules. Though it is ment for somewhat more experienced programmers I welcome new commers to try this out.

The idea is that one creates simplistic state diagrams in a program called 'yEd' and uses scripts to transform these state diagrams in fully compilable state machines. With a supplementing script the rest of the project can be assembled.

I also provide a tool to manage your IO. The idea of this is that you fill in a text file with a certain format and run a scipt to generate your IO.c and IO.h files.

There will be supplementing modules for debouncing IO, MCP23017 IO extenders, keypads, RTC modules and more. Most of this is still work in progress or yet to be started.

# My motivation
When I was still learning for my bachelor degree in electronics, I had to write a paper about a topic of choise and I wrote a paper about bug prevention. Programming without bugs is very difficult to do. And on the level on which we program it is next to impossible. There are however things we can do to prevent atleast some bugs.

When I started with my first 'real' job, I had to program a bicycle wheel taping machine. And I started out in C. Though the machine was not that complicated, I had difficulty doing the job. During development of the software for this machine and the following machines, a lot of time went into adjusting the structure of the software. The taper software currently still exists out of a switch-case which uses numbers for case-labels and where state++ is used to transist from state to another. The readability is poor and it is an annoying job to make changes. The software came with lots of bugs, of which alot were easily avoidable.

All software in the company was a complete mess. It was undocumentated, comments were in dutch and english. the states of their simplisitc state machines were not modulair. 'inc state' or in C 'state++' was used a lot instead of constants, every state had a label of letters and numbers instead of names and some files contained 20000 lines of code. It is still a payne to add states. There was no methods for an entry state. One-time-only stuff which had to happen for a state had to happen in the previous state. Adding a new state ment screwing arround with numbers and transplanting parts of codes. And the worst part, their version control was a complete mess. In some folders there were several extensions to be found; .ASM, .A, .BAC, and no extension. Alles files had the same content, on top of that the entire folder was copied so you'd have folderX (Copy 18) present. And yes 18 was the highest copy number I came accros.

With all my knowledge I obtained from coding myself, coding with other people, bugs, looking at folder (copy 18) and undocumentated and poorly written software. I realized that some drastic changes were needed. We needed order, extreme order. 

Some of the things I learned (though I mainly learned how not to do certain things):

- how to implement a well-written modulair state machine
- how to manage project files.
- how macros can actually increase readability
- how to work with git

When writing my paper about preventing bugs was, I learned that 'repeatability' was part of preventing bugs. With this I mean the ability to reproduce earlier achiefed results without bugs. And this triggered something within me. When taking all my learned lessons in account I came to the one logical conlusion, people make a lot of mistakes. The one logical solution? Code generation.

# Code generation.
The solution to tackle as much problems as possible is to generate as much code as possible. The simple python and shell scripts I have developed can do the following things:

- They can generate entire state machine skeletons using a simple state diagram made with yEd.
- They can assembly a fully arduino compilable projec folder
- They can create files for software timer
- IO can be generated using a simple text file

The scripts will be used in the alarm clock example to set up an entire new project. I will show all steps which must me made for setting up a project. For this we will use a checklist.

# My state machine implementation

## The basics, how does it work?

There are infinite ways to make a finite state machine or FSM. I have seen my share in terror in some of the wrong ways and in some of the complicated ways.

Therefor I devised a state machine which is as human readable as possible. I made compromises between readability, performance and space usages. 

Before I show some code I'll explain briefly how my state machines work. And why I have chosen for that manner.

My state machine exists out of a switch-case of which every case performs a function call to an other function, the state. This state function returns true when the state is finished. When that is the case, the switch-case will select a new function.

The decision which state is next to be executed is taken in the switch case. All other code (the states) are elsewhere. The advantage of this is that the entire flow of the SM is caught within a relative small switch-case which ususally fits in a single page.

The states themselfes consist out of 3 parts; the entryState, the onState and the exitState. The entry state is executed once on the first iteration of the state. From that point on, the onState will be executed as long as the exitFlag is not set. Once this flag is set, the exitState will be executed. The exitState will return true when it is done, this will signal the SM that the state is finished. Though we need 2 flags per SM to work, this structure allows for great versability. Especially with the SW timer implementation every state can have a time out.

## yEd state diagrams.
This is were all our new SW projects begin. We write down the basic operation in the form of simple state diagrams in the program yEd. This is as much needed as well as it is important. It provides the basic SW documentation which is vital to have once your program becomes complex. It helps you and your co-workers read and navigate through the code.

The state diagrams are needed for the code generation. yEd stores the diagrams in a .graphml in ascii format which will feed a python script to generate all state machine skeletons. This action is done once. When states are to be removed or added, that has to be done manually. It remains of great importance that you keep updating the diagram. It is even better to first modify the diagram before you modify the SW.

This will greatly increase the effectiveness of maintaining and altering SW.

It is important to remember that these state diagrams are kept as simple and orderly as possible. The reasons or state-transition-condtions which control the flow of the state machine should not be drawn in the state diagram because of these reasons:
- After the code is generated, the state diagram will act primarly as a reading guide. Therefor we should not add too much information
- The reasons or conditions are often not yet known at the moment you are creating the diagram.
- Because of the clarity of the state diagram, it is easier to read the conditions in the SW instead
- It may be that the conditions are too complex to try to draw them in the diagram.

Wheter all of these reasons are valid or not, if you add them all togather it would be illogical to try to draw the transition conditions.

There are a few rules which must be followed when creating the state diagram. 
- The use of camelCase is mandatory. 
- State names may not ever contain spaces or compiler-unfriendly characters
- Nested state machines (state machines which are to be called from out another) are to have atleast 1 sphere without an outgoing arrow.
- Nested state machines diagrams may contain more than 1 process but take note that only 1 can run at any given time.
- Main state machines diagrams (which are called from the main loop) should not contain spheres without outgoing arrows. It is technically possible and it does not hurt but it should be avoided non the less

Spheres without outgoing arrows will automatically enter an idle state and by my theory no main state machine should ever enter such a state. With the 1 arguement that they simply do not have to.

## Code for the states

The code uses four macros for. Some people claim that macro's are ever a bad idea, that they make code harder to read because you use non-standard words and you can create a debug hell for yourself and for others. Point 1 is simply not true. Point 2 on the other hand can be true, but this is also perfectly avoidable. You can test the macros before you start using them on 200+ places.

Also you can see macro's as library functions. If you need a library to use the I2C bus, LCD, a keypad or a digital sensor. You will not be looking in the library itself to see how the functions precisely work. You just want to use the functions and get the job done. The same can be true for some macro's. You can be content if you can use them without knowing how they precisely look like.

Macros are just a tool of the language just like functions are. You just have to use them right. The states I developed all look like:
```
State(stateName) { // the state 
	entryState {
		
	}
	onState {

		exitFlag = true; }
	exitState {
		
		return true; } }
...
...
State(someState) { // different macro for the state machine
	nextState(someOtherState,0); } // when 'someState' is finished, 'someOtherState' will run.
```

Given the information that you are looking at a state of a state machine which is proven to work. You should be able to figure out how to read it and how to use it. You do not have to know how the macro's precisely look like. The macros however can be found above the SM in code.

If we expand the macro's we would get:
```
static bit someStateF(void)() {
	if(runOnce) {
		
	}
	runOnce = false;  
	if(!runOnce) {

		exitFlag = true; }
	if(!exitFlag) return false; 
	else {
		
		return true; } }
...
...

break; case someState: if(someStateF()) {
	nextState(someOtherState, 0); }
```
You now see how the states or functions actually work. I believe that the macro method displays enough information in order to work with them. And it looks much more clear.

Notice that, in the expanded version, there is a captital F appended to the state name. For states I often use an enum. Mainly because the actual binary number of the state is not important. The state functions cannot have the same name as the enumerated states. Therefor we add the letter 'F' of function for the function and function call.

Also note that a 'new' variable appeared 'runOnce'. May it be clear that this variable is used to handle the entryState. 

The macro's hide the runOnce flag, they hide the F behind the state names, they transform a typical C function in a 'State' and they modify a switch-case into a state machine. I just hide the things that we do not have to see. We do not have to see that every case has a break, we don't have to see in every state how the entryState works again and again.

In short I move as much as 'irrelevant' information to the background as I deemed logical.

## the macros

Though I have already gave some hints on how they look like, here are the macro's:
```
#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exit) return false; else

#define State(x) static bit x##F(void) // for the states
#undef State

#define State(x) break; case x: if(x##F()) // for the state machine
#undef State
```
The macros are always directly above where they are used. So you can always find and read them above the states and state machines. That you don't have to know them, does not mean it is a bad idea to know them.

There is one tiny drawback of the macro's they add a little bit of overhead. The runOnce flag is cleared every iteration for as long as the state is active. The if statement behind it, is also a bit overhead. I may in fact alter this one. I believe that the if(!runOnce) can be deleted altogather.

I have also showed the macro for the state machine `State(x) break; case x: if(x##F())` . The call to a state is done from within an if-statement. As all states return false or true, the state machine will be immediatly aware when a state is finished. Once a state is finished, the body of the if-statement will be carried out. More about this in the next chapter.

## the code for the state machine
Now I'll discuss all code of the state machine and the code arround the state machine. The state machine on it's own is a function which uses a switch-case to call the state functions.

```
#define State(x) break; case x: if(x##F())
extern bit newWals(void) {
	if(enabled) switch(state){
		default: case newWalsIDLE: return true;

		State(powerOff) {
			nextState(startPosition, 0); }

		State(startPosition) {
			nextState(startMotor, 0); }
...
...
		break;}
	else if(!stateT) enabled = true;
	return false;}
#undef State
```
This is the entire state machine function. In the first lines you notice a variable called 'enabled' and the default case which returns true. The enabled flag allows the state machine to run. It is used to allow for inter-state delays. It may be that you want to wait an x ammount of time between states. This delay (non-blocking) is set by filling in a value for the 2nd arguement of the following function. 

```
static void nextState(unsigned char _state, unsigned char _interval) {
	state = _state;
	if(_interval) {
  		stateT = _interval;
		enabled = false; }
	runOnce = true; }
```
This function is called every time when a state is finished. The function handles the next state, 'runOnce' flag, 'exitFlag' flag and optionally the 'enabled' flag and the the state timer (stateT). How the enabled flag works, will be discussed in the software timer chapter.

The return function of the state machine is typically only used for nested state machines. Nested state machines typically come to an end. When they reach this end, they will jump to the IDLE state. The IDLE state will return true. This signal the 'caller' or 'parent state machine' that the nested state machine is finished. If the state != IDLE, the state machine ever returns false.

This feature is not present in 'main state machines'. Main state machines typically do not remain in an IDLE mode. They can but they don't have to return true or false as they are called from the main.

## The code for the timers.
At our work we were at first using a beatiful piece of SW timers. We had a function `start(foo, intervalTime)` and `trigger(bar, delayTime)`. These function would dynamically allocate a sw timer and hook it up with the address of the function. It worked well but later on we got into trouble with the overhead which came with it. Long story short... I copied the SW timer usage of our now-retired assembly programmer and translated it into C and performed some optimization. In this timer ISR he made blocks with a separate interfals for 2ms (<- ISR timer), 10ms and 100ms. Within these blocks he'd decrement all software timers.

The ISR looks as follows:
```
ISR(TIMER2_OVF_vect) {
static unsigned char _1ms, _10ms, _100ms;
_ms += 1;
//	add 1ms timers here

if(!(_1ms % 10)) { // if 10ms passed
	_1ms = 0;
	_10ms += 1;
	// add 10ms timers
	if(debounceT) debounceT -= 1;

if(!(_10ms % 10)) { // if 100ms passed
	_10ms = 0;
	_100ms += 1;
	// add 100ms timers
	if(stateT) stateT -= 1;

if(!(_100ms % 10)) { // if 1000ms passed
	_100ms = 0;
	// add 100ms timers
	if(timerWith100msInterval) timerWith100msInterval -= 1;
	if(anotherTimerWithaTooLongName) anotherTimerWithaTooLongName -= 1; // some dummy values to illustrate the use

}
}
}
```
Please do not be upset about the strange indentation style here. I found this to be more logical for this particular function than any other default style.

This method simple decrements those timers which aren't 0 yet. Because of the devision of timer bases, all timers can be of an 8-bit type. This safes memory and increases performance alike. 

The typical arduino method with millis() is to substract 2x 32 bit variables and compare the result with a 3rd 32 bit variable.

The best part of this SW timer is that it is generated for us. All main state machines have their own timer variables within the timer files. The name of these timers are the names of the state machines with the captial T appended. Furthermore, timers for default functions are also added. As of now I only have debounceT implemented to debounce buttons and or sensors.

This incredibly simple method has proven to be quite dynamic and easy to use. A list of the features:
- Every state can have it's own time out if needed
- You can build a non-blocking in-state delay for a certain state
- You can implement a delay between two following states (using the nextState() function).

### The time-out
A time out for a state can be needed depending on your use case. It can be realized as follows:
```
State(swingArmOutside) {
	entryState{
		stateT = 200; // 2000ms delay
		arm = 1; }	// we swing out an arm, and awaits it's sensor
	onState{
		if(sensor) {
			// sensor is made on time
			errorFlag = false;
			exitFlag = true; } 
		else if(!stateT) {
			// time has expired
			errorFlag = true;
			exitFlag = true; } }
	exitState{
		// no action need
		return true; } }
```
When this state is finished 'errorFlag' can be true or false. The state machine, which is now ready to pick a new state, may read this flag in order to determen which state it should select.
```
...
State(swingArmOutside) {
	if(errorFlag) 	nextState(ALARM,0);
	else 		nextState(swingArmInside,0); }
...
```

### The in-state delay
It may occur that you want a delay between the entry state and the on state. This can be easily achieffed as following:
```
State(swingArmOutside) {
	entryState{
		stateT = 100; } // 1 second delay 
	onState{
		if(!stateT) { // time has expired
			armOutside = 1;
			exitFlag = true; } }
	exitState{
		// no action need
		return true; } }
```

### The inter-state delay
My earlier SW relied heavily on delays between 2 following states. The machines mostly use pneumatic cylinders and it takes time for them to reach their positions. And because most cylinders have no sensors, timing was your only friend. With this SW timer usage. A delay between 2 states is not really needed as we have the tools for in-state delays. However it does not hurt to have inter-state delays. It allows for a little more complexity if you want, or you make it a little more simple. The manner of using an inter-state delay is just very simple and easy.

The inter-state delay is achieffed by setting a value in the 2nd arguement of nextState(). This arguement is defaulted to 0. By changing the 0 to something what is not 0 the function will do 2 things. 
```
static void nextState(unsigned char _state, unsigned char _interval) {
	state = _state;
	if(_interval) {
  		stateT = _interval;
		enabled = false; }
	runOnce = true; }
```

When interval != 0, the enabled flag is set at 'false' and the interval value is loaded in the timer. This timer decrements until it reaches 0.

When the enabled flag is false, the SM will not be called, instead the state timer is polled in the else-if statement. Once the timer reaches 0, 'enabled' will be set at 'true' and the next state will be executed.
```
else if(!stateT) enabled = true;
```

## nested state machines
One of the most powerful features this SW structure offers is the easy method to utilize nested state machines. The nested state machines are almost identical to the main state machines. There are a few subtile differences.
- Nested SMs will always come to an end or in other words reach the idle stat (remember the speheres without outgoing errors)
- Nested SMs return true when they have reached this end, otherwise the SM will return false.
- Nested SM have no timer allocated in the timer files. Instead the nested SM can make use of the calling main SM's or 'parent' SM's timer. 

Within the header file of the nested SM there lies a macro for it's own timer. The child SM's timer is to be #defined as his parent SM's timer. If the timer is not defined, an #error directive will inform the programmer of this when the programmer attempts to compile the project. 
```
#define childT  parentT // this must be added by the programmer
#ifndef childT
#error childT is not defined, define this as the parent's timer
#endif
```
There are 2 very important advantages of implementing nested state machines.
- They give you the ability to handle complex tasks without sacrificing overal readability
- They increase readability by moving relative unimportant code from a SM to a nested SM. This simply means of moving the unimportant states to a different file in which you never have to look again.

For intance. In the first complex machine (a wheel trueing machine) I programmed, about 50% of the primary state machine's states were concerend with rolling the wheels in and out. Both processes were moved to a nested state machine called 'wheelHandle'. The tasks were not that complex, they simply consumed to much important space.

A nested state machine needs a one time initialisation and it's main function is to be called. Because of the structure the function call will eventually return true. Calling a nested state machine from out an other looks as follows:
This is for rolling a wheel in the machine
```
State(rollWheelIn) {
	entryState{
		handleWheelSetState(rollIn); } // initialize the nested SM. The function is generated, rollIn is a state of the SM
	onState{
		if(handleWheel()) {
			exitFlag = true; } }
	exitState{
		// no action need
		return true; } }
```
And for rolling out a wheel:
```
State(rollWheelOut) {
	entryState{
		handleWheelSetState(rollOu); } // now we roll a wheel out
	onState{
		if(handleWheel()) { 	// same function call
			exitFlag = true; } }
	exitState{
		// no action need
		return true; } }
```
WheelHandle contains 2 parallel state machines. Using the (generated) ...setState() function. I can let this single SM act if it is 2 seperate SMs. The advantages are that they use the same timer (the one of the parent) and all the code for handling the wheel lies in their own files in which we don't have to look in again. We have to scroll through less code in the important SM which increases maintainability.



# Round Robin tasks
Most programs often have a need for certain functions which are to be called every program cycle. Think of reading the serial bus, polling an IO extender or checking an emergency switch. These functions are better known as round robin tasks. 

Some tasks are more important than others. Therefor I split the RR tasks in to high priority tasks and low priority tasks. The low priority tasks are handled one task at the time. Every program cycle only one low priority task and every high priority task will be performed. This is done in the function `processRoundRobinTasks()` And this function looks default as follows:
```
extern void processRoundRobinTasks(void) {
	static unsigned char taskCounter = 0;
	taskCounter ++;
	
	// HIGH PRIORITY TASKS
	readSerialBus();
	updateIO();  // module for I2C IO extender <- example
	CAN();	// module for CAN bus
	
	// LOW PRIORITY TASKS

	switch(taskCounter) {
		default: taskCounter = 0;

		case 0:
		debounceButtons();
		break;
		
		case 1:
		readEstop();	// emergency switch
		break;

		case 2:
		/* fill in a task */
		break; } }
```
It is a switch-case which every cycle increaes it's taskCounter. When the taskCounter reaches a value for which there is no case. The default case will default it to 0. And as there is no break statement case 0 will be immediatly executed. Because of the default case it is not needed to #define the ammount of tasks.

All you need to do to add an task is to simple add a new case and increment the case number. You don't have to be concerned with the taskCounter at all. 

It is important to know that this function is generated in the generateMain.py script (which obviously does more than generating just the main). You can easily modify the script. If every project of your's have the same round robin tasks you can simply add these in the script.
Than you don't have to 'retype' the same function calls over and over again.


# Modules
Modules or libaries actually exist to make our life easier. With Arduino we depend on them. The LCD library, I2C and the default serial library are just a few of them. Ofcourse it may be desirable that for your projects you may want to make your own libraries. I am working on my own libraries (or modules how I like to call them as my SW is supposed to be modulair). I store all my modules in the module sub-folder which lies in the main folder.

I perfer to keep your own separated libraries close to your project. You may want to use and modify them for a different uController platform. And they are every preserved in your sub folder (which you should have in git). I once put a library next to the arduino libraries and I lost it when I updated the Arduino SW.

With every project you build the ammount of modules you make, will grow. I'll share some information about the modules I have created for arduino so far.

## Debouncing buttons
This module does what is says. It handles the debouncing on buttons. It works with primarily with just 2 functions. The first function is called `updateButton()` and this handles the actual debouncing. The function is to be called as a low priority RR task and it works as follows:
- With a SW timer samples of all buttons (and/or sensors) will be taken and the old samples will be safed.
- If the new sample is a match with the old sample, the button state will take a new value. There are 4 different button states, RISING, FALLING, PRESSED or RELEASED.
- Once a new state is approved, it will first be compared with the previous state. If the new Sample differs with the previous state, it means that the state is either RISING or FALLING depening on the new sample. Otherwise if the new state is the same as the previous state, the button state is either PRESSED or RELEASED depending on the new sample.

The code of this function:
```
Button::unsigned char updateButton() {
	static bool oldSample = false, statePrev = false;
	bool newSample = digitalRead(pin);

	if(newSample == oldSample) {	// if the same state is detected atleast twice in 20ms...
	
		if(newSample != statePrev) { // if a flank change occured return RISING or FALLING
			statePrev = newSample;

			if(newSample)	return RISING; 
			else			return FALLING; }

		else {						// or if there is no flank change return PRESSED or RELEASED
			if(newSample)	return PRESSED; 
			else			return RELEASED; } }

	oldSample = newSample; }
```
If you want to read the state of a button, you can use the function `readButton()`. This function does do just 2 things
- The function returns the current state of the button
- If the state is RISING or FALLING the state will be set at RELEASED or PRESSED respectively.

A RISING or FALLING state is only lasts for 20ms. If you call `readButton()` within this time, the function will return the FALLING or RISING state just one time. The read functions will set the state to PRESSED or RELEASED if you call it when the state is still RISING or FALLING. If you do not call `readButton()` within this time, the button state will be automatically set at PRESSED or RELEASED by `updateButton()`. Afteral a button state cannot be RISING or FALLING indefinately.

The code:
```
Button::unsigned char readButton() {
	byte retValue = state;

	if(state == RISING) state = RELEASED; // take note I use a pull-up resistor hence RISING -> RELEASED
	if(state == FALLING)state = PRESSED; // rising or falling may be returned just once

	return retValue; }
```



## I2C I/O extenders
I am a great fan of the MCP23017 I2C extenders. I use them in almost every project. There are of course many libraries to be found to control these devices. But I have not yet found one which is as easy as my own module. 

At work I worked a lot with the CAN bus to control slave devices. Entire protocols were in place to automatically update outputs as well as inputs. Therefor I wrote this library with the same idea. In SW I want to be able to type `outputX = 1` or `b = inputY`. I do not want to remember what is an I2C IO and what is an arduino IO (with the exception of PWM, analog and interrupt pins). The library uses one extern function which is to be called as a high priority RR task. The function will check first which output has changed, and sends the change over the I2C bus. Than the function will poll the inputs of the devices and update the master's input bytes.

Another benefit is that this module used the IO files. The ammount of MCP23017 devices is dependend on the highest defined IO number. If the highest IO is 51, than the module knows it has 51 / 16 + 1 = 4 MCP23017 devices. 

There is one condition which has to be met for this to work. The hardware addresses of the slaves must be incremented from 0x20 to 0x28 in ascending order.

In short the benefits of this module are:
- master SW knows how much mcp devices are present depening on the highest listed IO.
- master SW automatically initializes (with init function) all MCP devices with the correct tri-state value for the IODIRx registers.
- You can set an output as simple as: `someOutput = 1` or `someOtherOut = 0`.
- You can read an input as simple as:`if(!someInput)` or `if(someInput == 1)`.
- outputs, inputs and pullup_inputs may be next to eachother without problems
- The module gets the neccesairy IO from the IO files.
- You can simply use this library without having to controll the I2C bus directly.

# I/O management

# Software documentation
All software is ever to be properly documentated. Especially the modules which have often dependencies to one another. The C/C++ header file system is IMO not the most prittiest solution I've seen. If you have many dependencies all the #include's can become messy. Therefor we need order in the form of documentation.

For modules you want a:
- general description
- list of dependencies 
- list of extern functions
- list of extern variables
- a usage guide. 

This is an stripped example of a C work project:
```
/* Light handler

Description:
	this functions handles all lights for all machine types
	There are 3 separate functions for different machine types of which one is compiled
	The 3 types are currently classified as: 
	* machines with orange lights
	* machines with a stack light (including white light)
	* machines with an orange and a blue light (HTC)

Dependencies:
	io.h
	basicMachineFunctions.h
	scheduler.h
*/

//extern functions:  
extern void lightHandler();

// extern variables:
extern bit automaticEnabled;
extern bit outOfRims;
extern bit blink;
extern bit testMode;

/*usage:
	The function Light handler can be called as low priority round robin task. It handles all lights for you
	It has it's own timer in scheduler. 
	The function needs startBool from basic functions to control the white light
	It uses the airPressureSensor to control the red light
	The automatic routine of the machine should set, "automaticEnabled" to turn on the green light
	And by setting 'blink' the orange light will blink.
	The flag testMode is used to burn the yellow light when the machine is in test testMode
*/
```
To use extern or 'global' variables is often not needed and not recommended. They are slightly more susceptable for bugs. But regardless they can be handy. But this discussion does not belong to the scope of this chapter.

If your module is written as a class, the formatting is already partially done for you. As the keywords `public` and `private` do some formatting for you. The general description as well as a usage explanation and dependencies remain the most important informatiob. Do not forget to add these in the header files. 

Complicated functions for complicated calculations and complicated tasks can always use extra comments in the source files. Though modules are often designed to just to be used, they may need altering on a bad day. If you have written some complex code at your best moment of clarity you will find yourself falling short if you do not remember how your code works and you need to look for a bug. 

I have written code to analize the spoke patterns of spoke bicycle wheels. It has become an extremely complex thing and it still needs more features. Thankfully almost every line has comment. Additionally I have written a local wiki page explaining the math behind the code. If others need to make alterations to this, than they can because I documentated it!

It is always a general good idea to build your code as it were intended for others. So add comments where needed and keep the code nice and tidy.

And if it was not clear yet. All state machines must every have a simple sphere diagram. This is an excellent reading guide.
