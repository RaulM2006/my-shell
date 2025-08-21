#include "my_shell.h"
#include "array.h"
#include "env.h"

#define MAX_SIZE 1024

extern char** environ;

void run_shell() {
    array_t* hst = create_array_t(free);
    char** myargs = (char**)malloc(sizeof(char*) * MAX_SIZE);
    array_t* env = create_array_t((DestroyF)destroy_env_var);
    init_shell_env(env);

    while (1) {
        char* input = NULL;
        read_input(&input);
        input[strlen(input) - 1] = '\0';
        
        char* original_input = strdup(input);
        
        tokenize(&myargs, input, env);
        
        if (strcmp(input, "exit") == 0) {
            printf("Bye bye!\n");
            free(input);
            free(original_input);
            break;
        }
         
        run_command(myargs, original_input, hst, env, 1);

        free_args(myargs);
        
        myargs = (char**)malloc(sizeof(char*) * MAX_SIZE);

        free(input);
        free(original_input);
    }
    free_args(myargs);
    destroy_array_t(hst);
    destroy_array_t(env);
    exit(0);
}

void read_input(char** input) {
    printf(">> ");
    size_t buf_size = MAX_SIZE;

    if (getline(input, &buf_size, stdin) == -1) {
        printf(">> Error reading input\n");
    }
}

void tokenize(char*** myargs, char* input, array_t* env) {
    int cnt = 0;
    char token_buf[MAX_SIZE];
    int len = 0;
    for (int i = 0; input[i]; ++i) {
         if (input[i] == ' ') {
            token_buf[len] = '\0';
            (*myargs)[cnt++] = strdup(token_buf);
            len = 0;
        } else if (input[i] == '$') { // variable expansion
            if (len > 0) {
                token_buf[len] = '\0';
                (*myargs)[cnt++] = strdup(token_buf);
                len = 0;
            }

            char name[MAX_SIZE];
            int var_len = 0;
            i++;
            while (input[i] && input[i] != ' ') {
                name[var_len++] = input[i++];
            }
            name[var_len] = '\0';
            --i;

            char* value = get_var(name, env);
            if (value) {
                (*myargs)[cnt++] = strdup(value);
            } else {
                (*myargs)[cnt++] = strdup("");
            }
        } else {
            token_buf[len++] = input[i];    
        }
    }
    
    if (len > 0) {
        token_buf[len] = '\0';
        (*myargs)[cnt++] = strdup(token_buf);
    }

    (*myargs)[cnt] = NULL;
}

void run_command(char** args, char* original_input, array_t* history, array_t* env, int should_save_to_history) {
    if (args == NULL) return;

    // Handle built-ins before execvp
    if (handle_builtin(args, history)) return;
    
    // Expand environment variables if needed, then execute command
    if (is_history_expansion(args[0])) {
        char* expanded = expand_history_command(history, args[0]);
        if (!expanded) return;

        char** recalled_args = (char**)malloc(sizeof(char*) * MAX_SIZE);
        tokenize(&recalled_args, expanded, env);

        if (should_save_to_history) {
            add(history, strdup(expanded));
        }

        run_command(recalled_args, expanded, history, env, 0); 

        free(expanded);
        free_args(recalled_args);
        return;
    }
    
    if (should_save_to_history) {
        add(history, strdup(original_input));
    }

    // Run external command
    execute_external(args, original_input);
}

void history(array_t* hist) {
    for (int i = 0; i < hist->size; ++i) {
        printf("\t%d\t %s\n", i + 1, (char*)hist->elems[i]);
    }
}

int is_history_expansion(char* cmd) {
    return cmd[0] == '!' && isdigit(cmd[1]);
}

char* expand_history_command(array_t* history, char* cmd) {
    int index = atoi(cmd + 1) - 1;
    char* hist_entry = history->elems[index];
    if (!hist_entry) {
        printf("No command available at %d\n", index + 1);
        return NULL;
    }
    return strdup(hist_entry); // caller must free
}

int handle_builtin(char** args, array_t* h) {
    // history command
    if (strcmp(args[0], "history") == 0) {
        add(h, strdup("history"));
        history(h);
        return 1; // handled
    }
    
    return 0; // not handled
}

void execute_external(char** args, char* original_input) {
    int status;
    int pid = fork();

    if (pid == 0) {
        execvp(args[0], args);
        fprintf(stderr, "%s: invalid command\n", original_input);
        
        exit(1); 
    } else {
        wait(&status);
    }
}

void init_shell_env(array_t* arr) {
    for (int i = 0; environ[i] != NULL; i++) {
        char* entry = environ[i];
        char* eq = strchr(entry, '=');
        if (!eq) continue;

        size_t name_len = eq - entry;
        char name[name_len + 1];
        strncpy(name, entry, name_len);
        name[name_len] = '\0';

        char* value = eq + 1;

        set_var(name, value, arr);
        export_var(name, arr);
    }
}

void free_args(char** args) {
    if (!args) return;
    for (int i = 0; args[i]; ++i) {
        free(args[i]);
    }
    free(args);
}
