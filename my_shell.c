#include "my_shell.h"
#include "dynamic_list.h"

#define MAX_SIZE 1024

void run_shell() {
    vector* hst = create_vector();
    char** myargs = (char**)malloc(sizeof(char*) * MAX_SIZE);
    
    while (1) {
        char* input = NULL;
        read_input(&input);
        input[strlen(input) - 1] = '\0';
        
        char* original_input = strdup(input);
        
        tokenize(&myargs, input);
        
        if (strcmp(input, "exit") == 0) {
            printf("Bye bye!\n");
            free(input);
            free(original_input);
            free(myargs);
            break;
        }
         
        run_command(myargs, original_input, hst, 1);

        free(input);
        free(original_input);
    }
    destroy_vector(hst);
    exit(0);
}

void read_input(char** input) {
    printf(">> ");
    size_t buf_size = MAX_SIZE;

    if (getline(input, &buf_size, stdin) == -1) {
        printf(">> Error reading input\n");
    }
}

void tokenize(char*** myargs, char* input) {
    char* p = strtok(input, " ");
    int cnt = 0;
    while (p != NULL) {
        (*myargs)[cnt++] = p;
        p = strtok(NULL, " ");
    }
    (*myargs)[cnt] = NULL;
}

void run_command(char** args, char* original_input, vector* history, int should_save_to_history) {
    if (args == NULL) return;

    // Handle built-ins before execvp
    if (handle_builtin(args, history)) return;

    if (is_history_expansion(args[0])) {
        char* expanded = expand_history_command(history, args[0]);
        if (!expanded) return;

        char** recalled_args = (char**)malloc(sizeof(char*) * MAX_SIZE);
        tokenize(&recalled_args, expanded);

        if (should_save_to_history) {
            push_back(history, strdup(expanded));
        }

        run_command(recalled_args, expanded, history, 0); 

        free(expanded);
        free(recalled_args);
        return;
    }
    
    if (should_save_to_history) {
        push_back(history, strdup(original_input));
    }

    // Run external command
    execute_external(args, original_input);
}

void history(vector* hist) {
    for (int i = 0; i < hist->size; ++i) {
        printf("\t%d\t %s\n", i + 1, (char*)element(hist, i));
    }
}

int is_history_expansion(char* cmd) {
    return cmd[0] == '!' && isdigit(cmd[1]);
}

char* expand_history_command(vector* history, char* cmd) {
    int index = atoi(cmd + 1) - 1;
    char* hist_entry = element(history, index);
    if (!hist_entry) {
        printf("No command available at %d\n", index + 1);
        return NULL;
    }
    return strdup(hist_entry); // caller must free
}

int handle_builtin(char** args, vector* h) {
    if (strcmp(args[0], "history") == 0) {
        push_back(h, strdup("history"));
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
