#include<stdio.h>
#include<unistd.h>
#include<limits.h>
#include <string.h>
#include<termios.h>
#include<stdlib.h>
#include<ctype.h>

#define ERROR -1
#define NOT_TERMINAL 0
#define SUCCESS 0
#define BREAK 1
#define LINESIZE  40
#define END 9
#define NOW 1
#define DRAIN 2
#define FLUSH 3

int erase();
int ctrlU(int *pos);
int ctrlW(int *pos, char line[]);
int ctrlD(int pos);
int ctrlG();
int editor();
int checkCtrlSym(char c, int *pos, char line[]);
int wordWrap(int *pos, char line[]);
int checkWrite(int err, int needNum);

int changeTerm(struct termios *term, struct termios *savedAttributes);
int getattr(struct termios *termAttr);
int setattr(struct termios *termAttr, int flag);

int main(){
        struct termios term, savedAttributes;
        int err = isatty(STDIN_FILENO);
        if (err == NOT_TERMINAL){
                perror("stdin not terminal");
                return ERROR;
        }
        err = isatty(STDOUT_FILENO);
        if (err == NOT_TERMINAL){
        	perror("stdout not terminal");
        	return ERROR;
        }
        err = getattr(&savedAttributes);
	
        err = changeTerm(&term, &savedAttributes);
        if (err != SUCCESS){
		setattr(&savedAttributes, NOW);
		return ERROR;
        }
        for (int i = 0; i < LINESIZE; i++){
        	err = write(STDOUT_FILENO, " ", 1);
        	err = checkWrite(err, 1);
		if (err != SUCCESS){
			setattr(&savedAttributes, NOW);
			return ERROR;
		}
        }
        err = write(STDOUT_FILENO, "|\n", 2);
        err = checkWrite(err, 2);
	if (err != SUCCESS) {
		setattr(&savedAttributes, NOW);
		return ERROR;
	}
        err = editor();
        
        if (err == ERROR){
        	setattr(&savedAttributes, NOW);
        	return ERROR;
        }
        
        err = setattr(&savedAttributes, NOW); //возвращаем прежний режим работы стандартного ввода терминала
        if (err == ERROR){
        	return err;
        }

        return 0;
}

int checkCtrlSym(char c, int *pos, char line[]){
	int err;
	switch(c){
		case CERASE: {
			if (*pos > 0){
				err = erase();
				if (err == ERROR) return ERROR;
				(*pos)--;
			}
			break;
		}
		case CKILL: {
			err = ctrlU(pos);
			if (err == ERROR) return ERROR;
			break;
		}
		case CWERASE: {
			err = ctrlW(pos, line);
			if (err == ERROR) return ERROR;
			break;
		}
		case CEOT: {
			err = ctrlD((*pos));
			if (err == BREAK) return END;
			break;
		}
		default: {
			err = ctrlG();
			if (err == ERROR) return ERROR;
			break;
		}
	}
	return SUCCESS;
}

int editor(){
	char line[LINESIZE+1];
	char c;
	int err;
	int pos = 0;
	int length;
	do {
		length = read(STDIN_FILENO, &c, 1);
		if (length == ERROR){
			perror("error in read");
			return ERROR;
		}
		if (c == '\n'){
			err = write(STDOUT_FILENO, &c, 1);
			
			err = checkWrite(err, 1);
			if (err != SUCCESS) return ERROR;
			
			pos = 0;
		} 
		else if (iscntrl(c) || !isprint(c)){
			err = checkCtrlSym(c, &pos, line);
			if (err == END){
				return SUCCESS;
			}
		}
		else {
			err = write(STDOUT_FILENO, &c, 1);
			
			err = checkWrite(err, 1);
			if (err != SUCCESS) return ERROR;
			
			line[pos] = c;
        		pos++;
		}
		if (pos == LINESIZE + 1){
			err = wordWrap(&pos, line);
			if (err == ERROR) {
				printf("error in wordwrap\n");
				return ERROR;
			}
		}
        } while (length == 1);
        
        if (length != 1) {
        	printf("length of simbol is not 1\n");
        	return ERROR;
        }
        
        return SUCCESS;
}

int wordWrap(int *pos, char line[]){
	int err;
	int newpos = 0;
	int position = *pos;
	int savpos = position;
	while (position > 0 && !isspace(line[position - 1])){
		position--;
	}
	if (position > 0){
		for (int i = position; i < savpos; i++){
			erase();
			line[newpos] = line[i];
			newpos++;
		}
		position = newpos;
		err = write(STDOUT_FILENO, "\n", 1);
		err = checkWrite(err, 1);
		if (err != SUCCESS) return ERROR;
		
		for (int i = 0; i < position; i++){
			err = write(STDOUT_FILENO, &line[i], 1);
			err = checkWrite(err, 1);
			if (err != SUCCESS) return ERROR;
		}
	}
	else {
		err = write(STDOUT_FILENO, "\n", 1);
		err = checkWrite(err, 1);
		if (err != SUCCESS) return ERROR;
	}
	*pos = position;
	return SUCCESS;
}

int checkWrite(int err, int needNum){
	if (err == ERROR){
		perror("write error");
		return ERROR;
	}
	if (err != needNum){
		perror("not enough characters printed");
		return ERROR;
	}	
	return SUCCESS;
}

int erase(){
	int err = write(STDOUT_FILENO, "\b \b", 3);
	err = checkWrite(err, 3);
	if (err != SUCCESS){
		perror("can't do Backspace");
		return ERROR;
	}
	return SUCCESS;
}

int ctrlU(int *pos){
	int err;
	while (*pos > 0){
		err = erase();
		if (err != 0){
			perror("can't do Ctrl+U");
			return ERROR;
		}
		(*pos)--;
	}
	return SUCCESS;
}

int ctrlW(int *pos, char line[]){
	int err;
	while (*pos > 0 && isspace(line[(*pos) - 1])){
		err = erase();
		if (err != 0){
			perror("can't do Ctrl+W");
			return ERROR;
		}
		(*pos)--;
	}
	while (*pos > 0 && !isspace(line[(*pos) - 1])){
		err = erase();
		if (err != 0){
			perror("can't do Ctrl+W");
			return ERROR;
		}
		(*pos)--;
	}
	return SUCCESS;
}

int ctrlD(int pos){
	if (pos == 0){
		return BREAK;
	}
	return SUCCESS;
}

int ctrlG(){
	int err = write(STDOUT_FILENO, "\a", 1);
	err = checkWrite(err, 1);
	if (err != SUCCESS){
		perror("cannot do BELL");
		return ERROR;
	}
	return SUCCESS;
}

int getattr(struct termios *termAttr){
        int err = tcgetattr(STDIN_FILENO, termAttr);
        if (err == ERROR){
                perror("error in tcgetattr");
                return ERROR;
        }
        return SUCCESS;
}

int setattr(struct termios *termAttr, int flag){
        int err;
        switch(flag){
                case NOW: {
                        err = tcsetattr(STDIN_FILENO, TCSANOW, termAttr); 
                        break;
                }
                case DRAIN: {
                        err = tcsetattr(STDIN_FILENO, TCSADRAIN, termAttr); 
                        break;
                }
                case FLUSH: {
                        err = tcsetattr(STDIN_FILENO, TCSAFLUSH, termAttr);
                        break;
                }
                default: {
                        perror("error flag for tcsetattr");
                        err = ERROR;
                        break;
                }
        }
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
	
        (*term).c_lflag &= ~( ICANON | ECHO );
        (*term).c_cc[VMIN] = 1;

        err = setattr(term, FLUSH);
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
