# My state machine implementation

---
## How it all started
---
To better explain what it is I am doing, I'd like to show the indivudal steps it took me before I made the final structure as it is today.

Like most beginners I started with a simple switch-case as state machine. At the time I wasn't even familiar with the state machine concept.

A short example:
```c
switch(state) {
case 0:
    // state code

    if(/*condition*/) state++;
    break;

case 1:
    // state code 

    if(/*condition*/) state++;
    break;

case 2:
    // state code 

    if(/*condition*/)       state = 0;
    if(/*other condition*/) state--;
    break;
}
```
When you start with this, you will eventually get a really long and unclear stretch of code. The biggest inconvenicence I was having, was the lack of names. You cannot use numbers in a state-machine. That really really sucks. I also had the need for some code which would only run once when entering a state. Therefor I used a bit called 'messageSet'. I printed the states on a monitor. So every state began with:
```c
if(!messageSet) {
    messageSet = true;
    print("state x");
}
```
Meanwhile I also started using state-names.
```c
enum states {
    crossingIdle,
    closeCrossing,
    openCrossing,
};
...
...

case crossingIdle:
    if(!messageSet) {
        messageSet = true;
        print("crossingIdle");
        // init something
    }
    // state code

    if(/*condition*/) state = closeCrossing;
    break;

case closeCrossing:
    if(!messageSet) {
        messageSet = true;
        print("closeCrossing");
        // init something
    }
    // state code 

    if(/*condition*/) state = openCrossing;
    break;

case openCrossing:
    if(!messageSet) {
        messageSet = true;
        print("state openCrossing");
        // init something
    }
    // state code 

    if(/*condition*/) state = crossingIdle;
    if(/*other condition*/) state = openCrossing;
    break;
}
```
You can imagine that you get an enormous amount of messy code when you start filling in such a state machine with code and you have 30+ states. Especially if you are sill using numbers.

To get more overhead I started creating functions for the state code. So every state was just calling a function.

```c
case crossingIdle:
    crossingIdleFunction();
    break;

case closeCrossing:
    closeCrossingFunction();
    break;

case openCrossing:
    openCrossingFunction();

}
```
Though this switch-case already was much more clear, it was still far from perfect. The code which picked the follow-up states was still in the state functions. So I started moving the parts which picked the next state back to the switch case.

```c
static void crossingIdleFunction(void) {
    if(!messageSet) {
        messageSet = true;
        print("state openCrossing");
        // init something
        ...
        ...
    }
    // state code 
    ...
    ...
}
..
..


case crossingIdle:
    crossingIdleFunction();
    if(/*condition*/) state = closeCrossing;
    break;

case closeCrossing:
    closeCrossingFunction();
    if(/*condition*/) state = openCrossing;
    break;

case openCrossing:
    openCrossingFunction();
    if(/*condition*/) state = crossingIdle;
    if(/*other condition*/) state = openCrossing;
    break;

}
```
At this point I did something really clever. I captured the flow of this state-machine in this small swtich-case. At the time I also learned about making state diagrams on paper and I noticed there was a 101 coralation between the spheres and the arrows and the states and the follow-up states.

I separated state code from code which controlled the flow. Now I had a state-machine and I had the state functions.

Meanwhile I got a new coworker from which I learned a couple of things about programming. I let the state function return a status. And the state machine would look like:
```c
case crossingIdle:
    if(crossingIdleFunction()) {
        nextState(closeCrossing, 0);
    }
    break;

case closeCrossing:
    if(closeCrossingFunction()) {
        nextState(openCrossing, 30);
    break;

case openCrossing:
    if(openCrossingFunction()) {
        if(/*condition*/) nextState(crossingIdle, 10);
        else              nextState(openCrossing, 20);
    }
    break;
}
```
The advantage of this is that we could remove more state-related code to the state functions. As you can see the first 2 states have no longer an if statement and a condition and the the last state only has one if-else and one condition to decide which of the states it could pick. You also see that the new state is now picked by a function called `nextState()`. This function would also reset the `messageSet` flag to handle the one-time-only code and a variable delay time could be picked to execute the next state. Most of our machines have pneumatic cylinders which took to operate. Therefor we often had to wait between states.

At this moment the state machine was much more better than the code with which I started out. From my co-worker I also learned a few things about macros. It turned out that macros can do much more than to #define constants and IO. 

The nessecity of a state-diagram also became obvious to me. So I started to look for programs which could help me with this.

---
## yEd state diagrams
---

Nowadays this is were all our new SW projects begin. We write down the basic operation in the form of simple state diagrams in the program yEd. This is as much needed as well as it is important. It provides the basic SW documentation which is vital to have once your program becomes complex. It helps you and your co-workers read and navigate through the code.

The state diagrams are needed for the code generation. yEd stores the diagrams in a .graphml in ascii format which will feed a python script to generate all state machine skeletons. This action is done once. When states are to be removed or added, that has to be done manually. It remains of great importance that you keep updating the diagram. It is even better to first modify the diagram before you modify the SW.

This will greatly increase the effectiveness of maintaining and altering SW. I will show 2 examples state diagrams of a machine before I started and when I was finished. This particular machine was written in Assembly. Yet the basic principles remained the same.

These are 2 main state machines for 2 rim (un)loading arms which run parallel.

If you look in detail you'll see that the first draught came close to the end diagram

![Pons1](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/pons1.png?raw=true)
![pons2](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/pons2.png?raw=true)

During code developing, the SMs became more and more complex. There were like 6 or 7 versions. These were not used for the generation part which was not yet finished at the time (yes I can also generate assembly code for work). That is why I have drawn 2 main state machines and a supplementing one on a single document.

Even though this was in assembly it remained extremely easy to make every alteration. The sphere diagrams were an indespensible tool for developing the program. And the state machine structure as I developed proved very effective, dynamic and modulair.


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
- State names may not ever contain spaces or c-unfriendly characters
- Nested state machines (state machines which are to be called from out another) are to have atleast 1 sphere without an outgoing arrow.
- Nested state machines diagrams may contain more than 1 process but take note that only 1 can run at any given time.
- Main state machines diagrams (which are called from the main loop) should not contain spheres without outgoing arrows. It is technically possible and it does not hurt but it should be avoided non the less

Spheres without outgoing arrows will automatically enter an idle state and by my theory no main state machine should ever enter such a state. With the 1 arguement that they simply do not have to.

---
## Code for the stateFunctions
---

The code heavily relies on macros. Some people claim that macro's are ever a bad thing, that they make code harder to read because you use non-standard words and you can create a debug hell for yourself and for others. Point 1 is simply not true. Point 2 on the other hand can be true, but this is also perfectly avoidable. You can test the macros before you start using them on 200+ places.

Also think of the following: you can think of macro's as library functions. If you need a library to use the I2C bus, LCD, a keypad or a digital sensor. You will not be looking in the library itself to see how the functions precisely work. No, you'll be looking in the provided example so how you van *use* the library. You just want to *use* the functions and get the job done. The same can be true for some macro's. You can be content if you can *use* them without knowing how they precisely look like.

Macros are just a tool of the language just like functions are. You just have to use them right. The states I developed all look like:
```c
stateFunction(stateName) { // the state 
    entryState {

    }
    onState {

        exitFlag = true;
    }
    exitState {

        return true;
    }
}

```

Given the information that you are looking at a state of a state machine which is proven to work. You should be able to figure out how to read it and how to *use* it. You do not have to know how the macro's precisely look like.

If we expand the macro's and reformat the whole a bit, we would get:
```c
static bit someStateF(void)() {
    if(runOnce) {
        runOnce = false;  
    }
    

    exitFlag = true; // ofcourse this should come behind some if

    if(!exitFlag) {
        return false; 
    }
    else {

        return true;
    }
}


```
You now see how the statesFunctions actually work. I believe that the macro method displays enough information in order to work with them. And it looks much more clear. The state is clearly devided into these three parts; entryState, onState and exitState.

Notice that, in the expanded version, there is a captital F appended to the state name. For states I often use an enum. Mainly because the actual binary number of the state is not important. The state functions cannot have the same name as the enumerated states. Therefor we add the letter 'F' of function to the constant for the state function and the function call.

Also note that a 'new' variable appeared 'runOnce'. May it be clear that this variable is used to handle the entryState. 

The macro's hide the runOnce flag, they hide the F behind the state names, they transform a typical C function in an actual 'stateFunction' and they modify a switch-case into a complete state machine. I just hide the things that we do not have to see. We do not have to see that every case has a break, we don't have to see in every state how the entryState works again and again.

In short I move as much as 'irrelevant' information to the background as I deemed logical.

---
## The macros
---

Here are all used macros:
```c
#define stateFunction(x) static bool x##F(void)
#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else
#define State(x) break; case x: if(x##F())
#define STATE_MACHINE_BEGIN if(!enabled) { \
	if(!readTrack1T) enabled = true; } \
else switch(state){\
	default: Serial.println("unknown state executed, state is idle");\
    state = readTrack1IDLE; \
    case readTrack1IDLE: return true;
#define STATE_MACHINE_END break;}return false;

//#define beginState
#ifndef beginState
#error beginState not yet defined
#endif
```
That you don't have to know them, does not mean it could hurt to know them.

There is one tiny drawback of the macro's they add a little bit of overhead. The runOnce flag is cleared every program cycle for as long as the state is active.

---
## The code for the state machine
---
Now I'll discuss all code of the state machine and the code arround the state machine. The state machine on it's own is a function which uses a switch-case to call the state functions. When we expand the macros and format the code:

```c
extern bool newWals(void) {
    if(!enabled) {
        if(!readTrack1T) enabled = true;
    }
    else switch(state) {
    default: 
        Serial.println("unknown state, state is idle"); 
        state = readTrack1IDLE; 
    case readTrack1IDLE: 
        return true;
        break; // obsolete I know

    case powerOff:
        if(powerOffF()) {
            nextState(startPosition, 0);
        }
        break;

    case startPosition:
        if(startPositionF()) {
            nextState(startMotor, 0);
        }
        break;
    ..
    ..

        break;
    }
    return false;
}
```
This is the state machine function with all macros expanded. It is not that complicated. Every case calls a function which has the same name of the case + F appended. The call is done from within an if-statement. As soon as the stateFunction returns true the next state will be picked.

The beginning of the state machine show some interresting things:
```c
if(!enabled) {
    if(!readTrack1T) enabled = true;
}
else switch(state) {
default: 
    Serial.println("unknown state, state is idle"); 
    state = readTrack1IDLE; 
case readTrack1IDLE: 
    return true;
```
The enabled flag is used in combination with the software timers. When nextState is called and the 2nd argument is not 0. The enabled flag will be cleared and the software timer will be loaded. This turns off the state machine untill the timer reaches 0. When the timer reaches 0, the enabled flag will be set and the state machine will carry out the next state.

```c
static void nextState(unsigned char _state, unsigned char _interval) {
    runOnce = true;
	exitFlag = false;
	if(_interval) {
		enabled = false;
		readTrack1T = _interval;
    } 
	state = _state;
}
```

The default label also has some purpose. In the extremely unlikely event that the state variable contains an undefined value, this will be printed on the monitor. But this should not happen. If an unknown state is entered the state will be set at idle.

The idle case is a feature which is mainly ment for nested state machines. If a state machine enters the idle state, the function of the state machine will return true. Otherwise the state machine will return false. This happens when a state machine enters a state which does not have an outgoing arrow. If a state has no outgoing arrow, the next state will always be the idle state. This also means that the state machine is finished with it's task.

In order to make the code look pritty and hide these 'small' details from our sight, we apply all macros and the state machine wil look like this:
```c
extern bool newWals(void) {
    STATE_MACHINE_BEGIN

    State(powerOff) {
        nextState(startPosition, 0); }

    State(startPosition) {
        nextState(startMotor, 0); }

    STATE_MACHINE_END
}
```
It looks significantly different. We hide almost all of the details now. These details are also not important. Important is that you know how to add states and alter the flow of the state machine. As long as you understand that you can copy a state, paste it and give it an other name, than you know enough.


---
## The code for the timers in more detail.
---
At our work we were at first using a beatiful piece of SW timers. We had a function `start(foo, intervalTime)` and `trigger(bar, delayTime)`. These function would dynamically allocate a sw timer and hook it up with the address of a function. It worked well but later on we got into trouble with the overhead which came with it. Long story short... I copied the SW timer usage of our now-retired assembly programmer and translated it into C and performed some optimization. In this timer ISR he made blocks with a separate interfals for 2ms (<- ISR timer), 10ms, 100ms and 1000ms. Within these blocks he'd decrement all software timers.

The ISR looks as follows:
```c
ISR(TIMER2_OVF_vect) {
static unsigned char _1ms, _10ms, _100ms;
// every ms
_ms += 1;


// every 10ms
if(!(_1ms % 10)) { _1ms = 0; _10ms += 1;

    if(debounceT) debounceT -= 1;

// every 100ms
if(!(_10ms % 10)) { _10ms = 0; _100ms += 1;

    if(stateT) stateT -= 1;

// every second
if(!(_100ms % 10)) {  _100ms = 0;

    if(timerWith100msInterval) timerWith100msInterval -= 1;
    if(anotherTimerWithaTooLongName) anotherTimerWithaTooLongName-= 1;


}
}
}
}
```
This method simple decrements those timers which aren't 0 yet. Because of the devision of timer bases, all timers can be of an 8-bit type. This safes memory and increases performance alike. 

The typical arduino method with millis() is to substract 2x 32 bit values and compare the result with a 3rd 32 bit value.

The best part of this SW timer is that it is generated for us. All main state machines have their own timer variables assigned within the timer files. The name of these timers are the names of the state machines with the captial T appended. Furthermore, timers for certain modules can also be added. As of now I only have debounceT implemented to debounce buttons and or sensors.

This incredibly simple method has proven to be quite dynamic and easy to use. A list of the features:
- Every state can have it's own time out if needed
- You can build a non-blocking in-state delay for a certain state
- You can implement a delay between two following states (using the nextState() function).
  
---
### The time-out
---
A time out for a state can be needed depending on your use case. It can be realized as follows:
```c
State(swingArmOutside) {
    entryState{
        stateT = 200; // 2000ms delay
        digitalWrite(arm, HIGH);// swing out arm, and await sensor
        }	
    onState{
        if(sensor) {  // sensor is made on time
            errorFlag = false;
            exitFlag = true;
        } 
        else if(!stateT) {  // time has expired
            errorFlag = true;
            exitFlag = true;
        }
    }
    exitState{
        // no action need
        return true;
    }
}
```
When this state is finished 'errorFlag' can be true or false. The state machine, which is now ready to pick a new state, may read this flag in order to determen which state it should select.
```c
State(swingArmOutside) {
    if(errorFlag) nextState(ALARM,0);
    else          nextState(swingArmInside,0); }
```
---
### The in-state delay
---
It may occur that you want a delay between the entry state and the on state. This can be easily achieved as following:
```c
State(swingArmOutside) {
    entryState{
        stateT = 100; } // 1 second delay 
    onState{
        if(!stateT) { // time has expired
            exitFlag = true; } }
    exitState{
        digitalWrite(arm, HIGH);
        return true; } }
```
The line `digitalWrite(arm, HIGH);` could also have been behind the line `if(!stateT)` instead of being in the exitState. Technically it does not matter. It is however more logical to perform the action in the `exitState`. The `onState`'s only job is to just monitor the time in this statte.


---
### The inter-state delay
---
My earlier SW relied heavily on delays between 2 following states. The machines mostly use pneumatic cylinders and it takes time for them to reach their positions. And because most cylinders have no sensors, timing was your only friend. With this SW timer usage. A delay between 2 states is not really needed as we have the tools for in-state delays. However it does not hurt to have inter-state delays. It allows for a little more flexibility if you want. The manner of using an inter-state delay is already explained in a previous chapter. So I'll repeat it for you.

The inter-state delay is achieved by setting a value in the 2nd argument of nextState(). This argument is defaulted to 0. By changing the 0 to something what is not 0 you will set a delay time before the next state is run.


---
## Nested state machines
---
One of the most powerful features this SW structure offers is the easy method to utilize nested state machines. The nested state machines are almost identical to the main state machines. The code structure is the same, the syntax is the same and they get their own files. There are a few subtile differences.
- Nested SMs will always come to an end or in other words the nested SMs will always reach the idle state eventually (remember the speheres without outgoing errors)
- Nested SM have no timer allocated in the timer files. Instead the nested SM can make use of the calling main SM's or 'parent' SM's timer. 

Within the source file of the nested SM there lies a macro for it's own timer. The child SM's timer is to be #defined as his parent SM's timer. If the timer is not defined, an #error directive will inform the programmer of this when the programmer attempts to compile the project. 
```c
#define childT  parentT // this must be added by the programmer
#ifndef childT
#error childT is not defined, define this as the parents timer
#endif
```
There are 2 advantages of implementing nested state machines.
- They give you the ability to handle extremely comples tasks
- They increase readability by moving relative unimportant code from a SM to a nested SM. This simply means of moving the unimportant states to a different SM in a different file in which you never have to look again.

For intance. In the first complex machine (a wheel trueing machine) I programmed, about 50% of the primary state machine's states were concerend with rolling the wheels in and out the machine. Both processes were moved to a nested state machine called 'wheelHandle'. The tasks were not even remotely complex, they simply consumed to much important space. You kept scrolling past this code many times to reach the parts you wanted to program.

A nested SM, like a main SM, needs a one time initialisation and it's main function is to be called. Because of the SM structure the function call will eventually return true. Calling a nested state machine from out an other looks as follows:
This is for rolling a wheel in the machine
```c
State(rollWheelIn) {
    entryState{
        handleWheelSetState(rollIn);
    } // initialize the nested SM.
    onState{
        if(handleWheel()) {
            exitFlag = true;
        }
    }
    exitState{
        // no action need
        return true;
    }
}
```
And for rolling out a wheel:
```c
State(rollWheelOut) {
    entryState{
        handleWheelSetState(rollOut);
    } // now we roll a wheel out
    onState{
        if(handleWheel()) { 	// same function call
            exitFlag = true;
        }
    }
    exitState{
        // no action need
        return true;
    }
}
```
WheelHandle contains 2 parallel state machines. Using the (generated) XXXsetState() function. I can let this single SM act if it is 2 seperate SMs. The advantages are that they use the same timer (the one of the parent) and all the code for handling the wheel lies in their own files in which we don't have to look in again. We have to scroll through less code in the important SM which increases maintainability.

![handleWheel](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/handleWheel.png)

The 2 bottom states have no outgoing arrows. This means that after these states are finished, the SM will default to the IDLE states which will signal the parent SM that the nested SM finished it's job.

```c
State(lowerInnerWheelStop) {
    nextState(wheelIsInMachine, 0); }

State(wheelIsInMachine) {
    nextState(HandleWheelIDLE, 0); } // <<--- idle

State(stopWheel) {
    nextState(waitNextMachine, 0); }

State(ejectWheel) {
    nextState(wheelIsOutmachine, 0); }

State(waitNextMachine) {
    nextState(ejectWheel, 0); }

State(wheelIsOutmachine) {
    nextState(HandleWheelIDLE, 0); } // <<--- idle
```
As soon as this state machines reaches the idle state. This state machine will signal the upper lying state machine that it is finished with this 'hidden' line of code:
```c
case HandleWheelIDLE: return true;
```
The upper lying statemachine fetches this 'true' in this previous shown if-statement
```c
onState{
    if(handleWheel()) { // function call to wheelHandle
        exitFlag = true; 
    }
}
```
