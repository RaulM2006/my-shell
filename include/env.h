#ifndef ENV_H
#define ENV_H

#include "array.h"

#include <unistd.h>
#include <string.h>
#include <linux/limits.h>

/**
 * Structure representing a shell environment variable.
 */
typedef struct {
    char* name;    /**< Variable name (e.g., "PATH"). */
    char* value;   /**< Variable value (e.g., "/usr/bin"). */
    int exported;  /**< 1 if exported to child processes, 0 otherwise. */
} env_var_t;

/**
 * Creates a new environment variable.
 * @param name The variable name (string is copied).
 * @param value The variable value (string is copied).
 * @return Pointer to a newly allocated env_var_t.
 *         Caller is responsible for freeing it with destroy_env_var().
 */
env_var_t* create_env_var(char* name, char* value);

/**
 * Frees memory associated with an environment variable.
 * @param var Pointer to the env_var_t to destroy.
 */
void destroy_env_var(env_var_t* var);

/**
 * Sets or updates an environment variable in the array.
 * If the variable already exists, its value is replaced.
 * @param name Variable name.
 * @param value Variable value.
 * @param arr Pointer to the environment array.
 */
void set_var(char* name, char* value, array_t* arr);

/**
 * Retrieves the value of a variable by name.
 * @param name Variable name.
 * @param arr Pointer to the environment array.
 * @return Pointer to the value string, or NULL if not found.
 */
char* get_var(char* name, array_t* arr);

/**
 * Marks a variable as exported so it is included in execve envp.
 * If the variable does not exist, it is created with an empty value.
 * @param name Variable name.
 * @param arr Pointer to the environment array.
 */
void export_var(char* name, array_t* arr);

/**
 * Removes a variable from the environment array.
 * @param name Variable name.
 * @param arr Pointer to the environment array.
 */
void unset_var(char* name, array_t* arr);

/**
 * Counts how many variables are marked as exported.
 * @param arr Pointer to the environment array.
 * @return Number of exported variables.
 */
int count_exported(array_t* arr);

#endif

