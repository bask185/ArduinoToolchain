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

            if(newSample)   return RISING; 
            else            return FALLING; }

        else {						// or if there is no flank change return PRESSED or RELEASED
            if(newSample)   return PRESSED; 
            else            return RELEASED; } }

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
