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

This will greatly increase the effectiveness of maintaining and altering SW. I will show 2 examples state diagrams of a machine before I started and when I was finished. This particular machine was written in Assembly. Yet the basic principles remained the same.

These are 2 main state machines for 2 rim (un)loading arms which run parallel.

If you look in detail you'll see that the first draught came close to the end diagram

![Pons1](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/pons1.png)
![pons2](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/pons2.png)

During code writing, the SMs became more and more complex. There were like 6 or 7 versions. These were not used for the generation part which was not yet finished at the time (yes I can also generate assembly code for work). That is why I have drawn 2 main state machines and a supplementing one on a single document.

Even though this was in assembly it remained extremely easy to make every alteration. The sphere diagrams were an indespensible tool for developing the state machines. And the state machine structure as I developed proved very effective and dynamic.


It is important to remember that these state diagrams are kept as simple and orderly as possible. The reasons or state-transition-condtions which control the flow of the state machine should not be drawn in the state diagram because of these reasons:
- After the code is generated, the state diagram will act primarly as a reading guide. Therefor we should not add too much information
- The reasons or conditions are often not yet known at the moment you are creating the diagram.
- Because of the clarity of the state diagram, it is easier to read the conditions in the SW instead
- It may be that the conditions are too complex to try to draw them in the diagram.

Wheter all of these reasons are valid or not, if you add them all togather it would be illogical to try to draw the transition conditions.

An example of a state machine followed by one nested state machine:
![taper machine main SM](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/nestedSM.png)

The following one is the 'stabilize' nested state machine which I call 'kraken' (element out of native language)
![stabilize nested SM](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/kraken.png)

The taping module is as almost as big as these 2 state machines togather. You can imagine how it would look like if it would be one single state machine. It would be rather big. Ofcourse with the sphere diagram you would still have no difficulty maintaining it, but to split some of the tasks into nested SMs the 'whole package' becomes much more clearer.

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

![handleWheel](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/handleWheel.png)

The 2 bottom states have no outgoing arrows. This means that after these states are finished, the SM will default to the IDLE states which will signal the parent SM that the nested SM finished it's job.
