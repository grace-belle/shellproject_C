/*  Class: COMP381
    Program: 
    Author: Grace Miller
*/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFSIZE 100
#define PIPE -1
#define READ 0
#define WRITE 1

static int wordSize = 25;
static char* word;
int c;

int getword() {
    int i = 0;

    if(c == '\n') {
        c ='\0';
        return 0;
    }
    
    while((c = getchar()) != EOF && c == ' ');
    if(c == EOF || c == '\n') return 0;
    while(c != EOF && !isspace(c)) {
        if (c == '|') {
            word[i++] = '\0';
            return PIPE;
        }

        word[i++] = c;

        if(i == wordSize) {
            wordSize += 10;
            word = (char*) realloc(word, wordSize * sizeof(char));
        }

        c = getchar();
    }
    word[i] = '\0';
    return i;
}

typedef struct command {
    char **args;
    int argSize;
    struct command *pipe;
} command;

command *getcomm() {
    command *comm = (command*) malloc(sizeof(command));
    comm->argSize = 10;
    comm->args = (char**) malloc(10 * sizeof(char*));
    comm->pipe = (command*) NULL;
    int i = 0, size;
    
    while((size = getword()) && size != PIPE) {
        comm->args[i++] = strcpy((char*)malloc((size + 1) * sizeof(char)), word);
        if(i == comm->argSize) {
            comm->argSize += 2;
            comm->args = (char**) realloc(comm->args, comm->argSize * sizeof(char*));
        }
    }
    comm->args[i] = (char*) NULL;
    if(size == PIPE) comm->pipe = getcomm();
    return comm;
}

void freecommand(command *comm) {
    if(!comm) return;
    int i;
    for(i = 0; comm->args[i];i++) free(comm->args[i]);
    free(comm->args);
    freecommand(comm->pipe);
    free(comm);
}

void runcommand(command *comm, int readPipe) {
    int rw[2];
    if(comm->pipe) pipe(rw);
    int child;
    if(!(child = fork())) {
        if(readPipe) {
            dup2(readPipe, 0);
            close(readPipe);
        }

        if(rw[READ]) {
            close(rw[READ]);
        }

        if(rw[WRITE]) {
            dup2(rw[WRITE], 1);
            close(rw[WRITE]);
        }

        
        execvp(comm->args[0], comm->args);
        fprintf(stderr, "error running command '%s'\n", comm->args[0]);
        exit(1);
    }

    if(rw[WRITE]) close(rw[WRITE]);
    int status;
    waitpid(child, &status, 0);
    if(comm->pipe) runcommand(comm->pipe, rw[READ]);
    if(rw[READ]) close(rw[READ]);
}

int main(int argc, char argv[]){
    int i;
    command *comm;
	word = (char*) malloc(wordSize * sizeof(char));
 
    printf("Enter a command: ")
    comm = getcomm();
    while (1) {
        if(comm->args[0]) {
            if (!strcmp(comm->args[0], "exit")) break;
            runcommand(comm, 0);
            freecommand(comm);
        }
        printf("Enter a command: "); //prompts user to enter a command
        comm = getcomm();
    }
    
    printf("Bye bye\n");

    return 0;
}