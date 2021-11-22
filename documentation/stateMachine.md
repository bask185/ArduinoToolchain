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
You can imagine that you get an enormous amount of messy code when you start filling in such a state machine with code and you have 30+ states. Especially if you are still using numbers.

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

Nowadays this is where my new SW projects begin. I write down the basic operation in the form of simple state diagrams in the program yEd. This is as much needed as well as it is important. It provides the basic SW documentation which is vital to have once your program becomes complex. It helps you and your co-workers read and navigate through the code.

The state diagrams are also needed for actual code generation. yEd stores the diagrams in a .graphml in ascii format which will feed a python script to generate all state machine skeletons. This action is done once. When states are to be removed or added, that has to be done manually. It remains of great importance that you keep updating the diagram. It is even better to first modify the diagram before you modify the SW.

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

There is only 1 rule which must be obeyed when making a state diagram.
- Names must be valid C, so don't uses space or other strange characters 

---
## Code for the stateFunctions
---

The code relies partly on macros. 
```c
stateFunction( stateName )  // the state function
{ 
    if( sm.entryState()     // runs once upon entering this state function
    {

    }
    if( sm.onState()        // runs continously until exit() is run
    {

        sm.exit() ;         // launches the exitState and terminates this state function
    }
    if( sm.exitState()      // is run once after exit() is called
    {

    }

    sm.endState() ;         // signals state machine when function is finished.
}

```


If we expand the macro we would get:
```c
static bool someStateF(void)() 

```

Notice that, in the expanded version, there is a captital F appended to the state name. For states I always use an enum. Mainly because the actual values of the state aren't important. The state functions cannot have the same name as the enumerated state constants. Therefor we add the letter 'F' of function to the constant for the state function as well as to the function call.


## The code for the state machine
---
Now I'll discuss all code of the state machine and the code arround the state machine. The state machine on it's own is a function which uses a switch-case to call the state functions. When we expand the macros and format the code:

```c
extern bool newWals(void)
{
    if( sm.run() ) switch( sm.getState() ) {    // STATE_MACHINE_BEGIN

    break ; case powerOff:                      // State(x)
        if( powerOffF() ) {
            nextState( startPosition , 0 ) ; }

    break ; case startPosition:
        if( startPositionF() ) {
            nextState(startMotor, 0 ) ; }
    ..
    ..

        break;                                  //  STATE_MACHINE_END
    }

    return sm.getState() ;
}
```
This is the state machine with all macros expanded. It is not much complicated. The state machine only calls the accompanying stateFunction. If this function returns true, a new state can be picked with ```nextState(x, y)``` with x being the new state and y being an optional delay time (non blocking)

```nextState()``` only sets a few variables. The delay is achieved by turning the enabled flag off and setting an interval

```c
void StateMachine::nextState( uint8_t _state, uint32_t _interval ) 
{
    exitFlag = 0 ;
    runOnce = 1 ;
    if( _interval )
    {
        enabled = 0 ;
        prevTime = millis() ;
        interval = _interval ;
    }
    state = _state ;
}
```
The method ```sm.run()``` handles the timer and the enabled flag
```c
uint8_t StateMachine::run()
{
    if( enabled == 0 )
    {
        if( millis() - prevTime >= interval )
        {
            enabled = 1 ;
        }
    }
    return enabled ;
}
```
If ```enabled``` is turned off, the state machine waits after the given interval has expired and turns ```enabled``` on again. If ```sm.run()``` returns true, the rest of the state machine is run.


In order to make the code look prittier, I chose to hide these minor details from our sight. When we apply all macros, the state machine wil look like this:
```c
extern bool newWals(void)
{
    STATE_MACHINE_BEGIN

    State( powerOff ) {
        nextState( startPosition, 0 ) ; }

    State(startPosition) {
        nextState( startMotor, 0) ; }

    STATE_MACHINE_END
}
```
It looks significantly different.  Important is that you know how to add states and alter the flow of the state machine. 

---
### The built-in timer function
---
The state machine class comes with a built-in timeout functionality. Under the hood it works with ```millis()```
```c
State( swingArmOutside )
{
    if( sm.entryState() )
    {
        sm.setTimeout( 2000 ) ; // 2s timeout time
        digitalWrite(arm, HIGH);// swing out arm, and await sensor
    }	
    if( sm.onState() )
    {
        if( sensor == true )    // sensor is made on time
        {
            errorFlag = false;  // clear error flag
            sm.exit() ;         // exit this
        } 
        else if( sm.timeout() ) // time has expired
        {
            errorFlag = true;   // set error flag
            sm.exit() ;         // exit this
        }
    }
    if( sm.exitState() )
    {
        // no action need
    }

    sm.endState() ;
}
```

When this state is finished 'errorFlag' can be true or false. The state machine, which is now ready to pick a new state, may read this flag in order to determen which state it should select.
```c
State( swingArmOutside ) {
    if( errorFlag ) nextState (ALARM, 0 ) ;
    else            nextState (swingArmInside, 0 ) ; }
```
There is also a supplementing ```repeat()``` function. You can use it as follows:

```c
State( repeatTest )
{
    if( sm.entryState() )
    {
        // no action need
    }	
    if( sm.onState() )
    {
        if( sm.repeat( 500 ) )
        {
            foobar() ;  // this code is repeated every 500ms
        }
    }
    if( sm.exitState() )
    {
        // no action need
    }

    sm.endState() ;
}
```

### The _inter-state_ delay

Most of our software use delays between two following states. The machines mostly use pneumatic cylinders and it takes time for them to reach their positions. And because most cylinders have no sensors, waiting a certain time, is the only way. A delay between 2 states is not really needed as we have the tools for in-state timeouts. However it does not hurt to have inter-state delays. It allows for a little more flexibility if you want. 

The inter-state delay is achieved by setting a value in the 2nd argument of nextState(). This argument is defaulted to 0. By changing the 0 to something what is not 0 you will set a delay time before the next state is run in milliseconds.

---
## Calling a state machine.
A called state machine always returns it's current state. The return value can be used as a method to let state machines communicate with eachother. And you can monitor if a state machine is finised. By default there is always an idle state present. The idle state always returns 0.

If you create a sphere diagram with a or several spheres or states have no outgoing arrows, than the state machine will automatically enter the idle state after the last states are executed. 

See below example, it consists out of two state-machines in a single file. This is done because both state-machines can not be run in the same time. They are ment for rolling in and out a bicycle wheel and this cannot occur at the same moment.

![handleWheel](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/handleWheel.png)

The 2 bottom states have no outgoing arrows. This means that after these states are finished, the SM will default to the IDLE states which will signal calling routine that the wheel has rolled in or out.

```c
State(lowerInnerWheelStop) {
    nextState(wheelIsInMachine, 0); }

State(wheelIsInMachine) {
    nextState(HandleWheelIDLE, 0); } // <<--- idle, generated by script

State(stopWheel) {
    nextState(waitNextMachine, 0); }

State(ejectWheel) {
    nextState(wheelIsOutmachine, 0); }

State(waitNextMachine) {
    nextState(ejectWheel, 0); }

State(wheelIsOutmachine) {
    nextState(HandleWheelIDLE, 0); } // <<--- idle generated by script
```
The state-machines can also be called from an other state machine. Below is one example of a stateFunctions which calls this wheel handle module. As soon as the wheelHandle state machine returns 0, the next state can be picked.

```c
stateFunction( rollIn )
{
    if( sm.entryState() )
    {
        handleWheel.setState( waitOutputRail) ;
    }
    if( sm.onState() )
    {
        if( handleWheel() == 0 )      // function call to wheelHandle
        {
            exitFlag = true; 
        }
    }
    if( sm.exitState() )
    {
        // no action needed   
    }

    sm.endState() ;
}
```
The stateMachineClass library is well documentated and easy to use. List of available methods:

```c
void    setState( uint8_t _state )  // sets state machine to run a state of choise
void    reboot()                    // this allows the entryState to run again, Can be usefull from time to time
uint8_t getState()                  // returns the current state
uint8_t entryState()                // to be called from inside an if-statement. Ensures that code is only run once when entering a state
uint8_t onState()                   // can be left out, simply returns 1 so code always run
uint8_t exitState()                 // runs code only after the exitFlag is set
void    exit()                      // triggers current state to run the exitState
uint8_t endState()                  // to be called on the bottom of every state function, signals the state machine if the state function is finished or not
void    setTimeout( uint32_t time2run ) // sets a timeout time in milliseconds
uint8_t timeout()                   // returns true when the time times out
void    nextState( uint8_t _state, uint32_t _interval ) // transitions from one state to another
uint8_t run()                       // this function is used to handle time delays between states, it is usually called via a macro
uint8_t repeat( uint32_t interval)  // keeps repeating code with _interval_ time, can be called from on onState
``` 