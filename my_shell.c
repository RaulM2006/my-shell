#include "my_shell.h"
#include "dynamic_list.h"

void run_shell() {
    vector* hst = create_vector();
    while (1) {
        char* input = NULL;
        read_input(&input);
        input[strlen(input) - 1] = '\0';
        
        char* original_input = strdup(input);
        
        push_back(hst, (char*)original_input);
        char** myargs = NULL;
        tokenize(&myargs, input);
        
        if (strcmp(input, "exit") == 0) {
            printf("Bye bye!\n");
            free(input);
            break;
        }
         
        run_command(myargs, original_input, hst);

        free(input);
        //free(original_input);
        free_args(myargs);
    }
    //destroy_vector(hst);
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
        (*myargs)[cnt++] = strdup(p);
        p = strtok(NULL, " ");
    }
    (*myargs)[cnt] = NULL;
}

void run_command(char** myargs, char* original_input, vector* h) {
    if (myargs == NULL) return;

    // Handle built-ins before execvp
    if (strcmp(myargs[0], "history") == 0) {
        history(h);
        return;
    }

    if (myargs[0][0] == '!') {
        char* args_copy = strdup(myargs[0] + 1);
        int index = atoi(args_copy) - 1;
        free(args_copy);

        char* cmd = element(h, index);
        if (!cmd) {
            printf("No command available at %d\n", index + 1);
            return;
        }

        char** recalled_args = NULL;
        tokenize(&recalled_args, cmd);
        run_command(recalled_args, cmd, h);
        //free(cmd);
        free_args(recalled_args);
        return;
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

void free_args(char** args) {
    if (args == NULL) return;
    for (int i = 0; args[i] != NULL; ++i) {
        if (args[i] != NULL) {
            free(args[i]);
        }
    }
    free(args);
}
