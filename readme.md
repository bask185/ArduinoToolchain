# State Machines & project management
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
