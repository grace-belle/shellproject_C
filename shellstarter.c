/*  Class: COMP381
    Prof: Prof. Ackerman

*/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFSIZE 100

static int wordSize = 25;
static char* word;
int newline = 0;

int getword() {
    if(newline) {
        newline = 0;
        return 0;
    }
    int c;

    int i = 0;
    while((c = getchar()) != EOF && c == ' ');
    if(c == EOF || c == '\n') return 0;
    while(c != EOF && !isspace(c)) {
        word[i++] = c;

        if(i == wordSize) {
            wordSize += 10;
            word = (char*) realloc(word, wordSize * sizeof(char));
        }
        c = getchar();
    }
    newline = c == '\n';
    word[i] = '\0';
    return i;

    //check for a space and call getword if | is found
}

static int argSize;
static char** args;

void getcomm() {
    int i = 0, size;
    char input;
    
    while(size = getword()) {
        args[i++] = strcpy((char*)malloc((size + i) * sizeof(char)), word);
        if(i == argSize) {
            argSize += 2;
            args = (char**) realloc(args, argSize * sizeof(char*));
        }
    }
    args[i] = (char*) NULL;
}

int main(int argc, char argv[]){
    int i;

	word = (char*) malloc(wordSize * sizeof(char));
    args = (char**) malloc(argSize * sizeof(char*));
    printf("Enter a command: "); //prompts user to enter a command
    getcomm();

    while (strcmp(args[0], "exit")) {    
        if(args[0]) {
            if(!fork()) {
                execvp(args[0], args);
                exit(1);
            }
            int status;
            wait(&status);
        }
        for (i = 0; args[i]; i++) free(args[i]);
        printf("Enter a command: ");
        getcomm();
    }
    printf("Bye bye\n");

    return 0;
}