#include "my_shell.h"

void run_shell() {
    while (1) {
        char* input = NULL;
        read_input(&input);
        input[strlen(input) - 1] = '\0';
        if (strcmp(input, "exit") == 0) {
            printf("Bye bye!\n");
            free(input);
            break;
        }

        char* original_input = strdup(input);
        
        char** myargs = NULL;
        tokenize(&myargs, input);
        
        int status;
        if (fork() == 0) {
            execvp(myargs[0], myargs);
            printf("%s: invalid command\n", original_input);
            exit(1);
        }
        wait(&status);
        free(input);
        free(original_input);
        free(myargs);
        input = NULL;
    }
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
    (*myargs) = (char**)malloc(sizeof(char*) * (strlen(input) / 2 + 2));
    char* p = strtok(input, " ");
    int cnt = 0;
    while (p != NULL) {
        (*myargs)[cnt++] = p;
        p = strtok(NULL, " ");
    }
    (*myargs)[cnt] = NULL;
}
