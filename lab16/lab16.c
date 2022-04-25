#include<stdio.h>
#include<unistd.h>
#include<limits.h>
#include <string.h>
#include<termios.h>
#include<stdlib.h>

#define BUFFSIZE 2
#define ERROR -1
#define NOTTERMINAL 0
#define SUCCESS 0

void clearBuf();
int wrongAnswer(char *c);
int readAnswer(char *answer);

int main(){
        char answer;
        struct termios term, savedAttributes;

        int err = isatty(STDIN_FILENO);
        if (err == NOTTERMINAL){
                perror("stdin error");
                exit(1);
        }

        printf("I have a question. Your answer should have a single character\nYour answer?\n\n");

        err = readAnswer(&answer);
        if (err == ERROR)
                return err;

        printf("Your answer is %c\n", answer);

        return 0;
}

void clearBuf(){
        char c;
        do{
                c = getchar();
        }
        while (c != '\n');

}

int readAnswer(char *answer){
        int length = read(STDIN_FILENO, answer, BUFFSIZE);
        if (length == ERROR){
                perror("error in read");
                return ERROR;
        }

        if (length != 1 && answer[BUFFSIZE-1] != '\n' || answer[0] == '\n'){
                length = wrongAnswer(answer);
        }
        if (length == ERROR)
                return ERROR;
}

int wrongAnswer(char *c){
        int length;
        do{
                clearBuf();
                printf("Length of your answer is not one\n");
                printf("Your answer should have a single character.\nRepeat your answer, please\n\n");
                length = read(STDIN_FILENO, c, BUFFSIZE);
                if (length == ERROR){
                        perror("error in read");
                        return ERROR;
                }
        }
        while (length != 1 && c[BUFFSIZE-1] != '\n' || c[0] == '\n');
}
     
