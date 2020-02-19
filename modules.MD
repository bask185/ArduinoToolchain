# Modules

Modules or libaries actually exist to make our life easier. With Arduino we depend on them. The liquid crystal library, I2C and the default serial library are just a few of them. Ofcourse it may be desirable that for your projects you may want to make your own libraries (I prefer to refer to my libraries as modules). 

I store all my modules in the module sub-folder in the state-machine-script folder. With every project you build the amount of modules you make, may grow. The idea is that every time you start a new project, the assemble script allows you to pick the modules you want to include in your new project and copy these modules to new project folder.

This is merely a choise of mine and it may not be nessecarily also the best choise for you. I make copies of the modules instead of using a fixed library location such as Arduino does. 

If in a project a module needs altering I have to copy the alteration in the state-machine-script folder IF I want to have this change for all future projects. If a module has a bug and 10 projects have this module, you need to redistribute the alteration to all 10 projects. I 

On the other hand. It may also be that a desired change is unique to that project. If you do the change in a shared folder you may break other projects. Imagine if you would alter the standard liquidCrystal library from Arduino so that it can be used to control a very unique LCD. Than you might break all your projects which are using that library.

I'd like to add just one thing. A module should like be tested and debugged before you start using it in 10 different projects. So the change that you ever want to distribute a change to 10 different projects is next to 0.

Making new modules may mean that the assembleProject script also needs alterations for a very special reason. For instance, if you pick module X to be included in your new project folder, you may want to call a certain function as a round robin task as well as a custom init function and assign a unique software timer. The assemble script can be modified to do precisely all of that. Than you don't have to do it yourself anymore therefor you cannot forget it anymore.

I have one example from my work:

We build machines which can produce bicycle and motor wheels. Almost every machine has atleast one orange light. Some machines have four lights. I made a light handler module. If I include this module, the script will make a call to the `lightHandler()` function from roundRobinTasks.cpp and include it's header files. There is also a timer asigned for this module called `blinkT` which is used to let the lights blink with a certain interval. The module has some dependenies like some IO (there must be an orangeLight output for instance). But when I include this module and make the IO I have to do practically nothing to controll the lights on the machine. If I want the orange light to blink, I can set a certain flag. The red and white lights are depended of the airPressureSensor input and the greenLight is controlled by another flag.

---
## Debouncing class
---
So far I have one module to debounce buttons and sensors. It is as of now (19/2/2020) still work in progress. Though the module itself is finished, I need to implent this in the io files.

This module does what is says. It handles the debouncing on inputs. It works with primarily with just 2 functions. The first function is called `updateInputs()` and this handles the actual debouncing. The function is to be called as a low priority RR task and it works as follows:
- With a SW timer samples of all inputs will be taken and the old samples will be safed.
- If the new sample is a match with the old sample, the input state will take a new value. There are 4 different button states, RISING, FALLING, ON or OFF.
- Once a new state is approved, it will first be compared with the previous state. If the new Sample differs with the previous state, it means that the state is either RISING or FALLING depening on the new sample. Otherwise if the new state is the same as the previous state, the button state is either ON or OFF depending on the new sample.

The code of this function:
```c
Debounce::unsigned char updateInput() {
    static bool oldSample = false, statePrev = false;
    bool newSample = digitalRead(pin);

    if(newSample == oldSample) {	
	
        if(newSample != statePrev) {
            statePrev = newSample;

            if(newSample)   return RISING; 
            else            return FALLING; }

        else {	
            if(newSample)   return ON; 
            else            return OFF; } }

    oldSample = newSample; }
```
If you want to read the state of aN INPUT, you can use the function `debounceInput()`. This function does do just 2 things
- The function returns the current state of the button
- If the state is RISING or FALLING the state will be set at RELEASED or PRESSED respectively.

A RISING or FALLING state is only lasts for 20ms. If you call `debounceInput()` within this time, the function will return the FALLING or RISING state just one time. The read functions will set the state to ON or OFF if you call it when the state is still RISING or FALLING. If you do not call `debounceInput()` within this time, the input state will be automatically set at ON or OFF by `debounceInput()`. Afteral an input state cannot be RISING or FALLING indefinately.

The code:
```c
Debounce::unsigned char debounceInput() {
    byte retValue = state;

    if(state == FALLING) state = OFF;
    if(state == RISING)  state = ON; 

    return retValue; }
```
---
## Future modules
---
In the future I will be expanding my module folder and push the new features to my git hub repository. I want to make atleast these modules for you to use:
  
- keypad matrix module
- ds3231 / ds1307 RTC module
- model train related modules
- - handcontroller
- - dcc central
- - accesoiry code


---
## Promoting a state machine to module
---
It may be that you on one good day you will write a state machine which can be used for other projects as well. For instances. I am currently programming a 6-state state machine to simulate real life day-night cycles with RGBWW led strips. I have also included a rain and a thunder state. This state machine uses a geekcreit dfplayer to play mp3 files for the rain and thunder soundeffects. Once this state machine is complete I can make a few alterations and put it in the module folder. Than I can modify the assembly script to initialize the thing, call it from the void loop and create the needed software timers.

I would litterly have to type 0 lines of C code and have a fully functioning well tested day night lighting simulation with sound as soon as the project folder is assembled.