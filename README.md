# Custom Shell programmed in C

## Description

This shell has been created using the unix system calls.
The shell is interactive and can be quit only by typing "quit" in the command prompt.

## Features

- background and foreground processes

- semicolon(;) separated list of commands 

- i/o redirection

- (multiple)piped commands 

- Commands:
```bash
cd
```
```bash
pwd
```
```bash
echo
```
```bash
ls [-a, -l,-la]
```
```bash
quit
```

- System commands with or without arguements

- User(self) defined commands:
```bash
pinfo
```
```bash
remindme
```
```bash
clock
```
```bash
setenv
```
```bash
unsetenv
```
```bash
jobs
```
```bash
kjob
```
```bash
overkill
```
```bash
bg
```
```bash
fg
```

- Signal handling:
	Signals like `ctrl+c` and `ctrl+z` are handled appropriately. The shell can be quit only by typing `quit`.

## Syntax for the user defined commands:

- pinfo:
```bash
$ pinfo
$ pinfo <pid>
```
- remindme:
```bash
$ remindme <time> <reminder>
```
- clock:
```bash
$ clock -t <time-step> -n <duration>
```
- setenv:
```bash
$ setenv var [value]
```
- unsetenv:
```bash
$ unsetenv var
```
- jobs:
```bash
$ jobs
```
- kjob:
```bash
$ kjob <jobNumber> <signalNumber>
```
- overkill:
```bash
$ overkill
```
- fg:
```bash
$ fg <jobNumber>
```
- bg:
```bash
$ bg <jobNumber>
```
## Working of various modules:

- parse.c :
	- Contains the functions to take the user input and parse it.

- builtin.c :
	- Contains the functions for all the user-defined commands.

- exed.c :
	- Contains the functions to handle background and foreground processed, executing builtin and other system commands.

- makefile.h :
	- contains all the global variables and the function declarations.


## Usage

Compile using the command: 
```bash
$ make
```
Execute using the command:
```bash
$ ./makefil
```
