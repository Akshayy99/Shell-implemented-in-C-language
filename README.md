## HASH SHELL MADE IN C

- HASH (Hot-Again Shell)  

## AUTHOR: Akshay Kharbanda (20171037)

This shell has been created using system calls.
The shell is interactive gnland can be quit only by typing "quit" in the command prompt.

It can handle the following:-
- background and foreground processes

- semicolon(;) separated list of commands 

- i/o redirection

- (multiple)piped commands 

- Commands:
	- cd
	- pwd
	- echo
	- ls (with flags "-a", "-l", and "-la"/"-al")
	- quit

- System commands with or without arguements

- User(self) defined commands:
	- pinfo
	- remindme
	- clock
	- setenv
	- unsetenv
	- jobs
	- kjob
	- overkill
	- bg
	- fg

- Signal handling:
	Signals like ctrl+c and ctrl+z are handled appropriately. The shell can be quit only by typing "quit".

## Syntax for the user defined commands:

- pinfo:
	- $ pinfo
	- $ pinfo <pid>

- remindme:
	- $ remindme <time> <reminder>

- clock:
	- $ clock -t <time-step> -n <duration>

- setenv:
	- $ setenv var [value]

- unsetenv:
	- $ unsetenv var

- jobs:
	- $ jobs

- kjob:
	- $ kjob <jobNumber> <signalNumber>

- overkill:
	- $ overkill

- fg:
	- $ fg <jobNumber>

- bg:
	- $ bg <jobNumber>

## Working of various modules:

- parse.c :
	- Contains the functions to take the user input and parse it.

- builtin.c :
	- Contains the functions for all the user-defined commands.

- exed.c :
	- Contains the functions to handle background and foreground processed, executing builtin and other system commands.

- makefile.h :
	- contains all the global variables and the function declarations.


## RUNNING THE SHELL

Compile using the command: - $ make

Execute using the command: - $ ./makefil
