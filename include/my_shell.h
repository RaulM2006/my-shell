#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#include "array.h"

/**
 * Main shell loop.
 * Reads user input, tokenizes, handles history and environment, and executes commands.
 * Terminates when the user types "exit".
 * Allocates and frees memory internally.
 */
void run_shell();

/**
 * Reads a line of input from stdin.
 * @param input Pointer to a char* that will be set to a newly allocated string.
 *              The caller is responsible for freeing the allocated memory.
 */
void read_input(char** input);

/**
 * Splits the input string into tokens separated by whitespace.
 * @param args Pointer to a char** array where tokens will be stored.
 *             Must be pre-allocated with enough space for tokens.
 * @param input The input string to tokenize. Modified in place by strtok.
 */
void tokenize(char*** args, char* input);

/**
 * Executes a command (builtin or external).
 * @param args Null-terminated array of tokenized arguments.
 * @param original_input The raw input string (used for error messages and history).
 * @param history Pointer to the command history array.
 * @param env Pointer to the environment variables array.
 * @param to_save 1 to save command in history, 0 otherwise.
 */
void run_command(char** args, char* original_input, array_t* history, array_t* env, int to_save);

/**
 * Prints the command history.
 * @param hist Pointer to the history array.
 */
void history(array_t* hist);

/**
 * Frees memory allocated for tokenized arguments.
 * @param args Null-terminated array of argument strings.
 */
void free_args(char** args);

/**
 * Checks whether a command is a history expansion.
 * @param cmd The command string.
 * @return 1 if the command starts with '!', 0 otherwise.
 */
int is_history_expansion(char* cmd);

/**
 * Expands a history reference (e.g. "!3") to the actual command.
 * @param history Pointer to the history array.
 * @param cmd The history reference string.
 * @return Newly allocated expanded command, or NULL if not found.
 *         Caller is responsible for freeing the returned string.
 */
char* expand_history_command(array_t* history, char* cmd);

/**
 * Handles built-in commands (set, unset, export, history, etc.).
 * @param args Tokenized arguments.
 * @param h Pointer to the history array.
 * @param env Pointer to the environment array.
 * @param original_input The raw input string.
 * @return 1 if the command was handled as a builtin, 0 otherwise.
 */
int handle_builtin(char** args, array_t* h, array_t* env, char* original_input);

/**
 * Executes an external command by forking and calling execve.
 * @param args Null-terminated argument vector.
 * @param env Pointer to the environment array.
 */
void execute_external(char** args, array_t* env);

/**
 * Initializes the shell environment from the inherited environ[].
 * @param arr Pointer to the environment array to populate.
 */
void init_shell_env(array_t* arr);

/**
 * Expands tokens such as environment variables ($VAR).
 * @param myargs Pointer to the tokenized argument array.
 * @param env Pointer to the environment array.
 */
void expand_tokens(char*** myargs, array_t* env);

/**
 * Sets or updates a shell variable.
 * @param args Tokenized arguments of the form VAR=VALUE.
 * @param env Pointer to the environment array.
 */
void set(char** args, array_t* env);

/**
 * Removes a variable from the shell environment.
 * @param args Tokenized arguments (variable names).
 * @param env Pointer to the environment array.
 */
void unset(char** args, array_t* env);

/**
 * Marks variables for export so they are passed to child processes.
 * @param args Tokenized arguments (variable names).
 * @param env Pointer to the environment array.
 */
void export(char** args, array_t* env);

void cd(char** args, array_t* env);

/**
 * Prints all environment variables stored in the shell.
 * @param env Pointer to the environment array.
 */
void print_env(array_t* env);

/**
 * Builds an envp[] array suitable for passing to execve.
 * @param env Pointer to the environment array.
 * @return Newly allocated NULL-terminated envp array.
 *         Caller is responsible for freeing it.
 */
char** build_envp(array_t* env);

/**
 * Searches for a command in the PATH.
 * @param cmd The command name.
 * @param env Pointer to the environment array.
 * @return Newly allocated string with the full path, or NULL if not found.
 *         Caller is responsible for freeing the returned string.
 */
char* find_in_path(char* cmd, array_t* env);

// TODO 
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

