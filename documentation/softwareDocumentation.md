# Software documentation
All half complex software should ever be properly documentated. Especially when the modules which have often dependencies to one another. The C/C++ header file system is IMO not the most prittiest solution I've seen. If you have many dependencies all the #include's can become messy. Therefor we need order in the form of documentation.

For modules/libaries you want a:
- general description
- list of dependencies 
- list of extern functions
- list of extern variables
- optinonally, a usage guide. 

This is an stripped example of a header file of a C project from my work:
```c
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
- The function Light handler can be called as low priority round robin task. It handles all lights for you
- It has it's own timer in scheduler. 
- The function needs startBool from basic functions to control the white light
- It uses the airPressureSensor to control the red light
- The automatic routine of the machine should set, "automaticEnabled" to turn on the green light
- And by setting 'blink' the orange light will blink.
- The flag testMode is used to burn the yellow light when the machine is in test test mode
*/
```

If your module is written as a class, the formatting is already partially done for you. As the keywords `public` and `private` do some of formatting for you. The general description as well as a usage explanation and dependencies remain the most important information. Do not forget to add these in the header files. 

Complicated functions for complicated calculations and complicated tasks could always use some extra comments in the source files. Some modules or libaries are not always well documentated, they may need altering on a bad day. If you have written some serious complex code at your best moment of clarity you will find yourself falling short when you do not remember how your code works and you need to look for a bug five years later. 

It is always a general good idea to build your code as it were intended for others. So add comments where needed and keep the code nice and tidy.

As mentioned in ```stateMachine.md``` you should always make yourself some kind of a sphere diagram or flowchart when building state machines. This is as important for yourself when you have to make alterations to your programs as it is for coworkers The sphere diagrams are also a very good tool to communicate to other coworks, even if they aren't programmers. It really helps to understand one another.

To provide more details to the functions or methods, so should use the doxygen method. This is a certain style for comments which has become a standard. It also comes with a feature. For examples you can look in ```stateMachineClass.cpp```.

One such example looks is this one
```c
/**
 * @brief function to handle the one time condition for the entry states
 *
 * @param N/A
 *
 * @return true or false
 */
uint8_t StateMachine::entryState()
{
    uint8_t retVal = runOnce ;
    runOnce = 0 ;
    return retVal ;
}
```
The format is important. Many modern text editors come with a certain previewer. If you call a certain function which is documented like this, you will see the description with highlighted words when you hover your mouse over the function.

