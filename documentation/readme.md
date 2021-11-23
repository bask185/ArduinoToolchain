# State Machines & Automated Project Assembly

The purpose of this github page is to provide you with the tools and knowledge to set up a 99% compile-able arduino folder complete with state machines skeletons, accompanying timers and other supplemental modules. Simple syntax makes it use-able for both beginning as well as experience programmers alike.

The main aims are:
- to help you setting up new projects with lightning speed, including build scripts, git repository and VS code tasks.
- to help you preventing bugs by generating as much source code as possible for you
- to offer you a generated powerfull state-machine structure to assist you with coding complex tasks.


# List of all features :

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
- date and time stamp are generated and hardcoded in the project before every compilation. Whenever your arduino runs it's program. The date/time of last compilation is sent to the serial monitor. 
- built in debug functionality within state machines
- Source code is compliant with Arduino's Hardware Abstraction Layer.

Further documentation is provided on this github page. You can find information about the software timers, round robin tasks and the state machines.


# Dependencies
- You need to clone or download this repository.
- You need python in order to run the python scripts
- optionally you need yEd to make state diagrams with if you plan to use state machines. One state diagram is included in this repository so you can test the scripts before you instal yEd. You can also generate new projects without state machines.

# Usage
Once you are set up, the basis usage is very simple. There are just two things you have to do to create a new project folder.
- Either create a state diagram IN yEd to the stateMachines folder or copy the example demo.graphml to this folder.
- To assemble a new project, you only have to double click and run 'assembleProject.py'. A black screen will appear which will ask you a few questions.

The script will prompt you for which hardware you will be coding for. This is important so the script can properly configure the build scripts. This script will also ask you if you want to include any modules from the module list. You can pick a module by entering the corresponding numbers. Type 'done' once you are finished and hit Enter again to close the program.

The script will generate source and header files, copy existing files and move some files to your new project folder. Any copies of your yEd diagrams are also moved to the new folder.

The assembled project ends up in the parent folder of the state-machine-script folder. Without added state machines, the assembled project is 100% compile-able.

The state machines however require one action before they can be compiled. Every state machine needs to have his 'beginState' defined. By default the #define is commented out which will trigger `#error` directive. If you try to compile the compile error will show you precisely what you must do. It is a fail-safe method which forces you to specify which state is to run first. 
```c
//#define beginState
#ifndef beginState
#error beginState not yet defined
#endif
```
Once this 'beginState' is defined your project is compile-able. From this point on you can start coding.

N.B.
There is one side note for the upload script. This script needs to know which comport you will be using. It wil automatically pick the last listed comport of your system. If you plug in your arduino board _before_ you assemble your project you have a really good chance that the script will pick the correct comport.




# IO generation
---
IO are managed in `io.tab`. A script called ```updateIO.py``` is used to generate the source files and header files. This always happens before compiling. This serves one single purpose.

I use the .tab file and the script so I can manage all IO in one single place. It is not uncommon for a programmer to forget to add the `pinMode()` instruction after adding a new IO. Manually changing IO also means editing two places in two places. Now you just have to alter one line in one place.

In `io.tab` you simply fill in a pin number followed by: tab, name/description, tab, iodir (OUTPUT, INPUT or INPUT_PULLUP) like:
```
7    sensorLeft    INPUT_PULLUP
```
The script will generate the source files. #defines are made for all descriptions in the header file and the source file will contain the ```initIO()``` function. This function sets the pinModes for us. Therefor we can not forget to type a pinMode instruction. ```initIO()``` is called from ```void setup()``` this part is also generated for us.

These IO files come with integrated support for the MCP23017 I2C IO extender. Making IO for these devices can be done in the exact same manner. There can be as much as 8 MCP23017 devices. Take note that every slave has only 16 GPIO. So use 0-15 for all IO pins. In order to use an MCP23017, you can enter the desired IO in io.tab underneath the reguar IO. You do need to label these parts with MCPx like in the following example.
```c
7    sensorLeft    INPUT_PULLUP

MCP1
15    sensorMidde    INPUT

MCP2
4    led4    OUTPUT
// ETC
```
```initIO()``` does the configuring of these devices for us. The tri-state registers are automatically configured correctly for you. To control the IO you can use two functions called ```mcpWrite(io, state);``` and ```mcpRead(io);```. They work the exact same as ```digitalWrite(io, state);``` and ``` digitalRead(io)``` The beauty is that we do not have to remember which IO is on which mcp device and on which port. These functions calculate the correct slave, port and pin numbers. Writing to a pin does not affect other pins. If you write to a pin with `mcpWrite()` it will first fetch the current state, than ORs and ANDs the current state with the desired IO change and sends it back.

The only condition is that the hardware adresses of the slaves must be in the correct order. That means that MCP1 must have hardware address 0x20, MCP2 must have address 0x21, etc 

# The new folder.
Your new folder has the following items in it:
- .vscode subfolder with tasks.json
- after compilation, the build folder with your binaries will appear (unless you use the IDE)
- build.sh for compiling
- upload.sh for comping and uploading
- giInit.sh which initializes a git repository, adds all files and makes a first commit.
- io.tab and updateIO.py for managing your IO
- your main xxx.ino file
- the stateDiagram folder with any stateDiagrams if you used it.
- src folder with supplementing files


## src/ folder.
The src/ folder in your new projects hold supplementing scripts as well as a couple of source files.

You will find files for the state machines, IO and there is the `macros.h` with some useful macros including my REPEAT_MS() macro.

Within the src subfolder there is also a python script called `addDate.py`. This script is always run before compiling and it generates `date.cpp` and `date.h`. Within `date.cpp` there is one function which prints the hardcoded time on the serial monitor. If an upload will fail, you should be able to see that in an incorrect timestamp.
```
void printDate()
{
	Serial.println("2021-11-23 16:16:01.401446") ;
}

```
If you have chosen to copy modules with the project those will also be inside the src folder.

