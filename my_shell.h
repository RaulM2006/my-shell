#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void run_shell();

// read input function 
void read_input(char**);

// method for separating the command given by user
void tokenize(char***, char*);

// TODO 
// refactor run_shell() code
// input validation handling
// command history
// env variable support
// support for background processes
// I/O redirection
// implement pipes
// handle signals
// support scripting
// tab completion
// job control
// custom shell variables
// config file


#endif
