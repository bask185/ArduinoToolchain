# Software documentation
All software is ever to be properly documentated. Especially the modules which have often dependencies to one another. The C/C++ header file system is IMO not the most prittiest solution I've seen. If you have many dependencies all the #include's can become messy. Therefor we need order in the form of documentation.

For modules you want a:
- general description
- list of dependencies 
- list of extern functions
- list of extern variables
- a usage guide. 

This is an stripped example of a C work project:
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
- The flag testMode is used to burn the yellow light when the machine is in test testMode
*/
```
To use extern or 'global' variables is often not needed and not recommended. They are slightly more susceptable for bugs. But regardless they can be handy. But this discussion does not belong to the scope of this project.

If your module is written as a class, the formatting is already partially done for you. As the keywords `public` and `private` do some of formatting for you. The general description as well as a usage explanation and dependencies remain the most important information. Do not forget to add these in the header files. 

Complicated functions for complicated calculations and complicated tasks can always use extra comments in the source files. Though modules are often designed to just to be used rather than to be read, they may need altering on a bad day. If you have written some complex code at your best moment of clarity you will find yourself falling short when you do not remember how your code works and you need to look for a bug. 

I have written code to analize the spoke patterns of spoke bicycle wheels. It has become an extremely complex thing and it still needs more features. Thankfully almost every line has comment. Additionally I have written a local wiki page explaining the math behind the code. If others need to make alterations to this, than they can because I documentated it!

It is always a general good idea to build your code as it were intended for others. So add comments where needed and keep the code nice and tidy.
