#include "my_shell.h"
#include "dynamic_list.h"

void run_shell() {
    vector* hst = create_vector();
    char** myargs = (char**)malloc(sizeof(char*) * 1024);
    
    while (1) {
        char* input = NULL;
        read_input(&input);
        input[strlen(input) - 1] = '\0';
        
        char* original_input = strdup(input);
        
        //push_back(hst, (char*)original_input);
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
    size_t buf_size = 1024;

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

void run_command(char** myargs, char* original_input, vector* h, int to_save) {
    if (myargs == NULL) return;

    // Handle built-ins before execvp
    if (strcmp(myargs[0], "history") == 0) {
        push_back(h, strdup("history"));
        history(h);
        return;
    }

    if (myargs[0][0] == '!' && isdigit(myargs[0][1])) {
        char* args_copy = strdup(myargs[0] + 1);
        int index = atoi(args_copy) - 1;
        free(args_copy);

        char* cmd = element(h, index);
        if (!cmd) {
            printf("No command available at %d\n", index + 1);
            return;
        }

        char* expanded = strdup(cmd);

        char** recalled_args = (char**)malloc(sizeof(char*) * 1024);
        tokenize(&recalled_args, expanded);
        
        if (to_save) {
            push_back(h, strdup(expanded));
        }
        run_command(recalled_args, expanded, h, 0); 

        free(expanded);
        free(recalled_args);
        return;
    }
    
    if (to_save) {
        push_back(h, strdup(original_input));
    }
    // Run external command
    int status;    
    int pid = fork();
    if (pid == 0) {
        execvp(myargs[0], myargs);
    
        printf("%s: invalid command\n", original_input);
        exit(1);
    }
    else {
        wait(&status);
    }
}

void history(vector* hist) {
    for (int i = 0; i < hist->size; ++i) {
        printf("\t%d\t %s\n", i + 1, (char*)element(hist, i));
    }
}
