#include<stdio.h>
#include<unistd.h>
#include <string.h>
#include<termios.h>
#include<stdlib.h>

#define BUFFSIZE 2
#define ERROR -1
#define NOT_TERMINAL 0
#define SUCCESS 0

void clearBuf();
int wrongAnswer(char *c);
int changeTerm(struct termios *term, struct termios *savedAttributes);
int readAnswer(char *answer);
int getattr(struct termios *termAttr);
int setattr(struct termios *termAttr);

int main(){
        char answer;
        struct termios term, savedAttributes;
        int err = isatty(STDIN_FILENO);
        if (err == NOT_TERMINAL){
                perror("stdin is not terminal");
                return ERROR;
        }
        err = getattr(&savedAttributes);
        if (err == ERROR)
                return ERROR;

        err = changeTerm(&term, &savedAttributes);
        if (err != SUCCESS){
                setattr(&savedAttributes);
                return ERROR;
        }
        printf("I have a question. Your answer should have a single character\nYour answer?\n\n");
        err = readAnswer(&answer);
        if (err == ERROR){
                setattr(&savedAttributes);
                return ERROR;
        }

        printf("Your answer is %c\n", answer);

        err = setattr(&savedAttributes);//tcsetattr(STDIN_FILENO, TCSAFLUSH, &savedAttributes); //возвращаем прежний режим работы стандартного ввода терминала
        if (err == ERROR){ 
                return ERROR;
        }
        return 0;
}


void clearBuf(){
        char c;
        do{
                c = getchar();
        }
        while (c != '\n');

}

int getattr(struct termios *termAttr){
        int err = tcgetattr(STDIN_FILENO, termAttr);
        if (err == ERROR){
                perror("error in tcgetattr1");
                return ERROR;
        }
        return SUCCESS;
}

int setattr(struct termios *termAttr){
        int err = tcsetattr(STDIN_FILENO, TCSAFLUSH, termAttr); //устанавливаем новый режим работы терминала немедленно
        if (err == ERROR){
                perror("error in tcsetattr");
                return ERROR;
        }
        return SUCCESS;
}

int changeTerm(struct termios *term, struct termios *savedAttributes){

        int err;
        struct termios bufTerm;

        *term = *savedAttributes;

        (*term).c_lflag &= ~( ICANON | ECHO ); //отключаем канонический режим и эхо
        (*term).c_cc[VMIN] = sizeof(char);

        err = setattr(term);
        if (err == ERROR)
                return ERROR;

        err = getattr(&bufTerm);
        if (err == ERROR)
                return ERROR;

        if ((*term).c_lflag != bufTerm.c_lflag || (*term).c_cc[VMIN] != bufTerm.c_cc[VMIN]){
                printf("Not all changes have been performed successfully");
                return ERROR;
        }
        return SUCCESS;
}

int readAnswer(char *answer){
        int length = read(STDIN_FILENO, answer, BUFFSIZE);
        if (length == ERROR){
                perror("error in read");
                return ERROR;
        }

        if (length != 1 && answer[BUFFSIZE-1] != '\n'){// || answer[0] == '\n'){
        ^Z      length = wrongAnswer(answer);
        }
        if (length == ERROR)
                return ERROR;
}

int wrongAnswer(char *c){
        int length;
        do{
                //clearBuf();
                printf("Length of your answer is not one\n");
                printf("Your answer should have a single character.\nRepeat your answer, please\n\n");
                length = read(STDIN_FILENO, c, BUFFSIZE);
                if (length == ERROR){
                        perror("error in read");
                        return ERROR;
                }
        }
        while (length != 1 && c[BUFFSIZE-1] != '\n');// || c[0] == '\n');
}
