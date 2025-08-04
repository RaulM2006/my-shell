#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "dynamic_list.h"
#include <ctype.h>

/**
 * Main shell loop: reads input, tokenizes, handles history, and executes commands.
 * Continues until the user types "exit".
 * Allocates and frees necessary memory internally.
 */
void run_shell();

/**
 * Reads a line of input from the user.
 * @param input Pointer to a char* that will be allocated and set to the input string.
 *              The caller is responsible for freeing the allocated memory.
 */
void read_input(char** input);

/**
 * Tokenizes the input string by spaces into an array of argument strings.
 * @param args Pointer to a char** array where tokens will be stored.
 *             The array must be pre-allocated with enough space.
 * @param input The input string to tokenize. This string is modified by strtok.
 */
void tokenize(char*** args, char* input);

/**
 * Executes a command based on the tokenized arguments.
 * Handles built-in commands, history expansion, and runs external programs.
 * @param args Tokenized command arguments.
 * @param original_input The original input string before tokenization.
 * @param history Pointer to the vector storing command history.
 * @param to_save Flag indicating whether to save this command in history (1 = yes, 0 = no).
 */
void run_command(char** args, char* original_input, vector* history, int to_save);

/**
 * Prints the command history stored in the vector.
 * @param hist Pointer to the vector holding history strings.
 */
void history(vector* hist);

/**
 * Frees memory allocated for the array of argument strings.
 * @param args Null-terminated array of argument strings to free.
 */
void free_args(char** args);

/**
 * Checks if a command string is a history expansion (starts with '!').
 * @param cmd Command string to check.
 * @return 1 if it is a history expansion, 0 otherwise.
 */
int is_history_expansion(char* cmd);

/**
 * Expands a history command (e.g. "!3") to the actual command string from history.
 * @param history Pointer to the vector holding history commands.
 * @param cmd The history expansion string (e.g. "!3").
 * @return Newly allocated string with the expanded command, or NULL if not found.
 *         Caller is responsible for freeing the returned string.
 */
char* expand_history_command(vector* history, char* cmd);

/**
 * Handles built-in commands like "history".
 * @param args Tokenized arguments.
 * @param history Pointer to the command history vector.
 * @return 1 if the command was a built-in and handled, 0 otherwise.
 */
int handle_builtin(char** args, vector* h);

/**
 * Executes an external command by forking and calling execvp.
 * @param args Tokenized arguments for the external command.
 * @param original_input The original input string (used for error messages).
 */
void execute_external(char** args, char* original_input);

// TODO 
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
