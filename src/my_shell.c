#include "my_shell.h"
#include "array.h"
#include "env.h"

#define MAX_SIZE 1024

extern char** environ;

void sigchild_handler(int sig) {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0);
}

void run_shell() {
    array_t* hst = create_array_t(free);
    char** myargs = (char**)malloc(sizeof(char*) * MAX_SIZE);
    array_t* env = create_array_t((DestroyF)destroy_env_var);
    init_shell_env(env);

    signal(SIGCHLD, sigchild_handler);

    while (1) {
        char* input = NULL;
        read_input(&input);
        input[strlen(input) - 1] = '\0';
        
        char* original_input = strdup(input);
        
        tokenize(&myargs, input);
        expand_tokens(&myargs, env); 

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
    fflush(stdout);
    size_t buf_size = MAX_SIZE;

    if (getline(input, &buf_size, stdin) == -1) {
        printf(">> Error reading input\n");
    }
}

void tokenize(char*** myargs, char* input) {
    int cnt = 0;
    char token_buf[MAX_SIZE];
    int len = 0;
    for (int i = 0; input[i]; ++i) {
         if (input[i] == ' ') {
            token_buf[len] = '\0';
            (*myargs)[cnt++] = strdup(token_buf);
            len = 0;
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
    if (handle_builtin(args, history, env, original_input)) return;
    
    // Expand environment variables if needed, then execute command
    if (is_history_expansion(args[0])) {
        char* expanded = expand_history_command(history, args[0]);
        if (!expanded) return;

        char** recalled_args = (char**)malloc(sizeof(char*) * MAX_SIZE);
        tokenize(&recalled_args, expanded);
        expand_tokens(&recalled_args, env);

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
    execute_external(args, env);
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

int handle_builtin(char** args, array_t* h, array_t* env, char* original_input) {
    // history command
    if (strcmp(args[0], "history") == 0) {
        add(h, strdup("history"));
        history(h);
        return 1; // handled
    }
    
    // set command
    if (strcmp(args[0], "set") == 0) {
        add(h, strdup(original_input));
        set(args, env);
        return 1; // handled
    }
    
    // unset command
    if (strcmp(args[0], "unset") == 0) {
        add(h, strdup(original_input));
        unset(args, env);
        return 1; // handled
    }

    // export command
    if (strcmp(args[0], "export") == 0) {
        add(h, strdup(original_input));
        export(args, env);
        return 1; // handled
    }

    // env command
    if (strcmp(args[0], "env") == 0) {
        add(h, strdup(original_input));
        print_env(env);
        return 1; // handled
    }

    // cd command
    if (strcmp(args[0], "cd") == 0) {
        add(h, strdup(original_input));
        cd(args, env);
        return 1; // handled
    }
    return 0; // not handled
}

void execute_external(char** args, array_t* env) {
    char* path = find_in_path(args[0], env);
    if (!path) {
        fprintf(stderr, "%s: command not found\n", args[0]);
        return;
    }
    
    int background = 0;
    int i = 0;
    char* current;
    while (args[i] != NULL) {
        current = strdup(args[i]);
        if (current && strcmp(current, "&") == 0) {
            background = 1;
            args[i] = NULL;
            break;
        }
        i++;
    }

    // build envp
    char** envp = build_envp(env);
    
    int status;
    int pid = fork();

    if (pid == 0) {
        // child
        execve(path, args, envp);
        perror("execve");
        exit(1);
    } else {
        if (!background) 
            waitpid(pid, &status, 0);
    }
    free(path);
    for (int i = 0; envp[i] != NULL; ++i) {
        free(envp[i]);
    }
    free(envp);
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

void expand_tokens(char*** myargs, array_t* env) {
    for (int i = 0; (*myargs)[i] != NULL; ++i) {
        char* token = (*myargs)[i];

        char expanded[MAX_SIZE];
        int out = 0;

        for (int j = 0; token[j] != '\0'; j++) {
            if (token[j] == '$') {
                char name[MAX_SIZE];
                int var_len = 0;
                
                int k = j;
                k++;
                while (token[k] && (token[k] >= 'A' && token[k] <= 'Z') && token[k] != ' ') {
                    name[var_len++] = token[k++];
                }
                name[var_len] = '\0';

                char* value = get_var(name, env);
                if (value) {
                    int vlen = strlen(value);
                    memcpy(&expanded[out], value, vlen);
                    out += vlen;
                } else {
                    expanded[out++] = ' ';
                }
                j = k - 1;
            } else {
                expanded[out++] = token[j];
            }
        }

        expanded[out] = '\0';
        free((*myargs)[i]);
        (*myargs)[i] = strdup(expanded);
    }
}

void free_args(char** args) {
    if (!args) return;
    for (int i = 0; args[i]; ++i) {
        free(args[i]);
    }
    free(args);
}

void set(char** args, array_t* env) {
    // set with no arguments
    if (args[1] == NULL) {
        for (int i = 0; i < env->size; ++i) {
            env_var_t* var = (env_var_t*)env->elems[i];
            printf("%s=%s\n", var->name, var->value);
        }
    } else {
    // set with arguments
        char* eq = strchr(args[1], '=');
        if (!eq) {
            printf("set: invalid argument\n");
            return;
        }

        *eq = '\0';
        char* name = args[1];
        char* value = eq + 1;

        set_var(name, value, env);
    }
}

void unset(char** args, array_t* env) {
    // unset with no arguments
    if (args[1] == NULL) {
        printf("unset: not enough arguments\n");
        return;
    }

    // unset with arguments 
    for (int i = 1; args[i] != NULL; ++i) {
        char* name = args[i];
        unset_var(name, env);
    }
}
        
void export(char** args, array_t* env) {
    if (args[1] == NULL) {
        printf("export: not enough arguments\n");
        return;
    }
    
    for (int i = 1; args[i] != NULL; ++i) { 
        char* name = args[i];
        char* eq = strchr(args[i], '=');
        if (eq) {
            // export with name and value as argument 
            *eq = '\0';
            char* name = args[i];
            char* value = eq + 1;
            set_var(name, value, env);
        }
        
        export_var(name, env);
    }
}

void cd(char** args, array_t* env) {
    char* target;
    if (args[1] == NULL) {
        target = get_var("HOME", env);
    } else if (strcmp(args[1], "-") == 0) {
            target = get_var("OLDPWD", env);
        } else {
                target = args[1];    
            }

    printf("target is %s\n", target);
    if (target == NULL) {
        perror("cd: target not set\n");
        return;
    }

    char* oldpwd = get_var("PWD", env);

    if (chdir(target) == -1) {
        perror("cd");
        return;
    }

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return;
    }
    if (oldpwd)
        set_var("OLDPWD", oldpwd, env);
    set_var("PWD", cwd, env);
}

void print_env(array_t* env) {
    for (int i = 0; i < env->size; ++i) {
        env_var_t* var = env->elems[i];
        if (var->exported) {
            printf("%s=%s\n", var->name, var->value);
        }
    }
}

char** build_envp(array_t* env) {
    int n = count_exported(env);

    char** envp = malloc((n + 1) * sizeof(char*));
    if (!envp) return NULL;
    
    int j = 0;
    for (int i = 0; i < env->size; ++i) {
        env_var_t* var = env->elems[i];
        if (var->exported) {
            int len = strlen(var->name) + 1 + strlen(var->value) + 1;
            envp[j] = malloc(len);
            if (!envp[j]) continue;
            snprintf(envp[j], len, "%s=%s", var->name, var->value);
            j++;
        }
    }

    envp[j] = NULL;
    return envp;
}

char* find_in_path(char* cmd, array_t* env) {
    if (strchr(cmd, '/')) {
        if (access(cmd, X_OK) == 0) return strdup(cmd);
        return NULL;
    }
    
    char* path = get_var("PATH", env);
    if (!path) path = "/bin:/usr/bin"; // fallback

    char* paths = strdup(path);
    char* token = strtok(paths, ":");
    while (token) {
        size_t len = strlen(token) + 1 + strlen(cmd) + 1;
        char* full = malloc(len);
        snprintf(full, len, "%s/%s", token, cmd);
        if (access(full, X_OK) == 0) {
            free(paths);
            return full; // found
        }
        free(full);
        token = strtok(NULL, ":");
    }

    free(paths);
    return NULL;
}

