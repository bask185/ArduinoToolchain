In this example I will make a state machine of which every state tests the debouncing states of a button. The PRESSED and RELEASED states are only to be printed 4x per second. For the hardware you simple need an arduino with a switch.

Because a button can have 4 different states, I'll make a state machine with 4 states. We will be using the serial monitor to display the button state as well tansitions between states.

# Make an yEd state diagram
The first step is to create a state diagram in yEd:

![alt txt](https://raw.githubusercontent.com/bask185/State-Machine-Scripts/master/images/buttonTest.png)

Note that every state can transisiton to every other state. That is why there are 2 arrows in both directions between 2 states.

Once the state diagram is finished you can run the python script 'stateMachineGenerator.py'.
The output of this script looks as follows:
```
0 = alarmClock.graphml
1 = buttonTest.graphml
2 = menu.graphml      
select state machine  
1
Is this state machine a 'main' or 'nested' type state machine?
main
main type selected
```
You are prompted to select a state machine from the list, then you are asked if it is a main state machine or a nested one.
When the script is finished you need to run generateMain.py. This scripts prompts you a name, creates the directory, generates the needed files and moves the whole to the new folder. The new folder is on the same level as the folder from which you build your projects. So if you put the state-machine-script folder in my documents, you should now have a new folder in my Documents. Mine is called buttonTest.

Once the folder and files are assembled we only need to fill in the IO.tab files and generate the needed IO.h and IO.c files before we can start with the actual coding. 

As we have just 1 IO. The content of the IO.tab will be:
```
Pin	 name	tri-state
3	switch	INPUT_PULLUP
13	ledPin	OUTPUT
```
I also added the well known led pin 13. Perhaps we can do something with it.

From now on we only have to fill in the state machine and the states. To summarize the tasks:
- each state needs to display a message over the serial bus upon entering the state
- each state must display 1 out of 4 button states, depending on the state of the button
- The PRESSED and RELEASED states are only to be printed 4x per second
- Switching or transitioning between states can be done by sending a certain character ('a','b','c','d') via the serial monitor
- When exiting a state the states must display another message
- We want to wait 1 second before a new state is executed.

# Filling in the state machine.
If a state can transition in more than 1 state (when 1 sphere has more than 1 outgoing arrow) we need to fill in the transition condition or 'flow' conditions. The generated SM looks as follows:
```
#define State(x) break; case x: if(x##F())
extern void buttonTest(void) {
	if(enabled) switch(state){
		default: case buttonTestIDLE: return;

		State(a) {
			nextState(b, 0);
			nextState(d, 0);
			nextState(c, 0); }

		State(b) {
			nextState(a, 0);
			nextState(c, 0);
			nextState(d, 0); }

		State(c) {
			nextState(d, 0);
			nextState(b, 0);
			nextState(a, 0); }

		State(d) {
			nextState(a, 0);
			nextState(c, 0);
			nextState(b, 0); }

		break;}
	else if(!buttonTestT) enabled = true; }
#undef State
```
Because we use serial communication to control this, we will use the global variable 'serialByte' from the serial module to fill in the flow conditions. As you have seen, the state names are idiotic short. They are just one character each. Therefor I will use the corresponding ascii values from the serial bus. I want an delay of 1 second during a state transisiton. To accomplish this I changed all '0's to '100's.
This will nicely show how the inter-state delay looks like.

The filled in state machine looks like:
```
#define State(x) break; case x: if(x##F())
extern void buttonTest(void) {
	if(enabled) switch(state){
		default: case buttonTestIDLE: return;

		State(a) {
			if(serialByte == 'b')	nextState(b, 100);
			if(serialByte == 'd')	nextState(d, 100);
			if(serialByte == 'c')	nextState(c, 100); }

		State(b) {
			if(serialByte == 'a')	nextState(a, 100);
			if(serialByte == 'c')	nextState(c, 100);
			if(serialByte == 'd')	nextState(d, 100); }

		State(c) {
			if(serialByte == 'd')	nextState(d, 100);
			if(serialByte == 'b')	nextState(b, 100);
			if(serialByte == 'a')	nextState(a, 100); }

		State(d) {
			if(serialByte == 'a')	nextState(a, 100);
			if(serialByte == 'c')	nextState(c, 100);
			if(serialByte == 'b')	nextState(b, 100); }

		break;}
	else if(!buttonTestT) enabled = true; }
#undef State
```
Don't forget that any of the nextState function are called when the current state exists. Any state may only exit when we send any of the 3 possible characters. If we send the character 'z' and we exit the state, strange things will happen.

# Filling in the states
Now we fill in the code for the first state 'a'. The empty generated state looks like:
```
State(a) {
	entryState {
	
	}
	onState {
	
		exitFlag = true; } }
	exitState {
	
		return true; } }
```
Remember that we want this state to display a message upon entering and leaving the state. We want to continously print the button state and we must be able to exit the state.

I first add the messages:
```
State(a) {
	entryState {
		Serial.println("entering state A, displaying the RISING state of switch");	}
	onState {
	
		exitFlag = true; } }
	exitState {
		Serial.print("leaving state A, next state will be ");
		Serial.println(serialByte);
		return true; } }
```
When I exit the state I will also display to which state we are going to jumb, using the 'serialByte'. Ofcourse this state can not yet work before the onState is filled in. By default, the exitFlag is automatically set in the `onState`. In the event that you do not fill anything in in the `onState`the state will run once and exits. So even if you forget to fill in a state, the state machine will go on.

Lets first be able to properly exit the state. State 'a' can switch to state 'b', 'c' or 'd'. Therefor we only need to listen for these 3 characters.
```
State(a) {
	entryState {
		Serial.println("entering state A, displaying the RISING state of switch");	}
	onState {
		if(serialByte) switch(serialByte) {  // if is used to detect if there is a byte to be read in
			default: break;		// any unknown byte will break out the switch case
			
			case 'b':
			case 'c':
			case 'd':
			exitFlag = true;	// the cases are used as a fall-through switch case. Any of these 3 chars will set the exitFlag
			break; } }
	exitState {
		Serial.print("leaving state A, next state will be ");
		Serial.println(serialByte);
		return true; } }
```
I found the fall-through switch case quite elegant really. You may even want to put the 3 cases behind one-another to reduce some space but you may upset your co-workers with it.

```
onState {
	if(serialByte) switch(serialByte) {  // if is used to detect if there is a byte to be read in
		default: break;		// any unknown byte will break out the switch case
			
		case 'b': case 'c': case 'd': // be care full not to burn yourself by pissing off co-workers
		exitFlag = true;	// the cases are used as a fall-through switch case. Any of these 3 chars will set the exitFlag
		break; } }
```
Lastly we need to print read and print the button state. This can be done easily done by adding the following line somewhere in the `entryState`.
```
if(button.readButton() == RISING) Serial.println("RISING");
```

With this line added, state 'a' is now finished. State 'a' now looks as follows:

```
State(a) {
	entryState {
		Serial.println("entering state A, displaying the RISING state of switch");	}
	onState {
		if(button.readButton() == RISING) Serial.println("RISING");
	
		if(serialByte) switch(serialByte) {  // if is used to detect if there is a byte to be read in
			default: break;		// any unknown byte will break out the switch case
			
			case 'b': case 'c': case 'd':
			exitFlag = true;	// the cases are used as a fall-through switch case. Any of these 3 chars will set the exitFlag
			break; } }
	exitState {
		Serial.print("leaving state A, next state will be ");
		Serial.println(serialByte);
		return true; } }
```
The states which will display RISING and FALLING will only print this just one time. Reading a RISING or FALLING state will automatically set the state to RELEASED or PRESSED respectively. If the states which displays the PRESSED and FALLING states were written exactly like state 'a', the states would print the states as fast the the program runs. To have some delay between the printing actions we will use our generated and assigned timer called 'buttonTestT'. 

When we implement the timer, state 'b' (which will print the PRESSED state) will look as follows:
```
State(b) {  // changed a in b
	entryState {
		Serial.println("entering state B, displaying the PRESSED state of switch");	}  // changed A in B
	onState {
		if(!buttonTestT && button.readButton() == PRESSED) { // added a check if the timer is 0
			Serial.println("PRESSED");
			buttonTestT = 25; } // 250ms delay -> 4x printing actions per second
	
		if(serialByte) switch(serialByte) { 
			default: break;		
			
			case 'a': case 'c': case 'd': // changed b in a
			exitFlag = true;	
			break; } }
	exitState {
		Serial.print("leaving state B, next state will be "); // changed A in B
		Serial.println(serialByte);
		return true; } }
```
State 'b' is almost identical to state 'a'. I only had to do 2 things. I had to use the timer and swap out an 'a' for a 'b' at a few places.

Now I welcome you to fill in the remaining two states yourself. You can also try to let one of the states print all 4 different button states. Or perhaps you can do something fancy with the onboard led.

# Last bits and pieces
After you filled in the remaining 2 states, there are three more things you have to do:
Firstly. The file roundRobinTasks needs to call the function updateButtons() as a low priority RR task. This function lies in IO.c and the IO.h is already #included. 

N.B. this step will be obsolete in the future because we will be able to select which module we want to import. This we can do from out the 'generateMain' script. If the 'buttonClass' module is imported, 'updateButtons()' will be generated within the roundRobinTasks.

Secondly. The state machine itself needs to be initialized. We do this by calling the generated function with the proper arguement 'a'.
In the setup of the main file call `buttonTestSetState(a)`. This will set the state at 'a' and sets the 'enable' flag. This will allow for our state machine to run and start in state 'a'. The states are extern, meaning that these are accessable by other files. For the same reason more state machines cannot have the same names for states. 

N.B. this step will also become obsolete. The idea is you only have to #define the beginState. The initializing function will be generated within setup()

Lastly. Every generated timer of a state machine is defaulted at a time base of 10ms. This can be changed to a base of 1ms or a base of 100ms by modifying the file 'scheduler.c'. In our particulair use-case we have a delay of 250ms and a delay of 1000ms. The values we used are '25' and '100' respectively and this works for the default timer base of 10ms.

When you have verified that our timer has the correct time base, we can compile the program and upload it to the arduino. Connect your switch between pin 3 and ground and open the serial monitor and you are good to go. (and ofcourse make sure the baud rate of the monitor is set @ 115200 BPS, I always use this baudrate). 

N.B. yes this last step will eventually also become obsolete. The plan is to prompt the time interval before generating the state machine. 

If you did everything right (like not making typos), your program should work 'right out of the box'. You should have no bugs and no unexpected behaviour (if you have not forgotten to set all state names or lettres correctly).

If you have followed through the example and you have not made any mistakes I expect you to have seen the advantages of this SW. 

What I personally like about this structure is that the most parts are generated for you and that the operation is so simple. It uses 8-bit decrementing timers, macros hide irrelevant code and lets everything look like states and state machines and the entire program structure is generated for you. You just have to fill in some states and flow conditions. 

Just plug your code in and play with the code!
