#ifndef ENV_H
#define ENV_H

#include "array.h"

#include <unistd.h>
#include <string.h>

typedef struct {
    char* name; 
    char* value;
    int exported; // should this be passed to execve()? 
} env_var_t;

env_var_t* create_env_var(char* name, char* value);
void destroy_env_var(env_var_t* var);

void set_var(char* name, char* value, array_t* arr);
char* get_var(char* name, array_t* arr);
void export_var(char* name, array_t* arr);
void unset_var(char* name, array_t* arr);

#endif
