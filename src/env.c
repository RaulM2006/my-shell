#include "env.h"

env_var_t* create_env_var(char* name, char* value) {
    env_var_t* new_var = (env_var_t*)malloc(sizeof(env_var_t));

    new_var->name = strdup(name);
    new_var->value = strdup(value);
    new_var->exported = 0;

    return new_var;
}

void destroy_env_var(env_var_t* var) {
    free(var->name);
    free(var->value);
    free(var);
}

void set_var(char* name, char* value, array_t* arr) {
    for (int i = 0; i < arr->size; ++i) {
        env_var_t* ev = (env_var_t*)arr->elems[i];
        if (strcmp(ev->name, name) == 0) {

            free(ev->value);
            ev->value = strdup(value);
            return;
        }
    }

    // not found, must store the variable
    env_var_t* new_env_var = create_env_var(name, value);
    add(arr, new_env_var);
}

char* get_var(char* name, array_t* arr) {
    for (int i = 0; i < arr->size; ++i) {
        env_var_t* ev = (env_var_t*)arr->elems[i];
        if (strcmp(ev->name, name) == 0) {
            return ev->value;
        }
    }
    return NULL;
}

void export_var(char* name, array_t* arr) {
    for (int i = 0; i < arr->size; ++i) {
        env_var_t* ev = (env_var_t*)arr->elems[i];
        if (strcmp(ev->name, name) == 0) {
            ev->exported = 1;
            return;
        }
    }

    // if not found, create with empty value and export
    env_var_t* new_var = create_env_var(name, "");
    new_var->exported = 1;
    add(arr, new_var);
}

void unset_var(char* name, array_t* arr) {
    for (int i = 0; i < arr->size; ++i) {
        env_var_t* ev = (env_var_t*)arr->elems[i];
        if (strcmp(ev->name, name) == 0) {
            del(arr, i);
            return;
        }
    }
}

