#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#define ERROR -1
#define CHILD_PROCCESS 0
#define SUCCESS 0 
#define FOR_READ 0
#define FOR_WRITE 1

int pipeReader(int *fd){
	char c;
	int err, length;
	do {
		length = read(fd[FOR_READ], &c, 1);
		if (length == ERROR){
			perror("read error");
			return ERROR;
		}
		c = toupper(c);
		
		err = write(STDOUT_FILENO, &c, 1);
		if (err == ERROR){
			perror("write error");
			return ERROR;
		}
	} while (length > 0);
	
	err = close(fd[FOR_READ]);
	if (err == ERROR){
		perror("close error");
		return ERROR;
	}
	
	return SUCCESS;
}

int pipeWriter(int *fd){
	char c;
	int err, length;	
	do{
		length = read(STDIN_FILENO, &c, 1);
		if (length == ERROR){
			perror("read error");
			return ERROR;
		}
		
		err = write(fd[FOR_WRITE], &c, 1);
		if (err == ERROR){
			perror("write error");
			return ERROR;
		}
	} while(length > 0);
	
	err = close(fd[FOR_WRITE]);
	if (err == ERROR){
		perror("close error");
		return ERROR;
	}
	
	return SUCCESS;
}

int readerChild(int *fd){
	int err;
	
	err = close(fd[FOR_WRITE]);
	if (err == ERROR){
		perror("close error");
		return ERROR;
	}	
	err = pipeReader(fd);
	if (err == ERROR){
		return ERROR;
	}
	
	return SUCCESS;
}

int writerChild(int *fd){
	int err;
	
	err = close(fd[FOR_READ]);
	if (err == ERROR){
		perror("close error");
		return ERROR;
	}
	err = pipeWriter(fd);
	if (err == ERROR){
		return ERROR;
	}
	return SUCCESS;
}

void closeBothfd(int *fd){
	int err = close(fd[FOR_READ]);
	if (err == ERROR){
		perror("close error");
	}
	int err = close(fd[FOR_WRITE]);
	if (err == ERROR){
		perror("close error");
	}
}

int main(){
	int fd[2];
	int err;
	err = pipe(fd);
	if (err == ERROR){
		perror("pipe error");
		return ERROR;
	}

	pid_t pid = fork();
	if (pid == ERROR){
		perror("fork error");
		closeBothfd(fd);
		return ERROR;
	}
	if (pid == CHILD_PROCCESS){
		err = readerChild(fd);
		if (err == ERROR)
			return ERROR;
		return SUCCESS;
	}
	
	pid = fork();
	if (pid == ERROR){
		perror("fork error");
		closeBothfd(fd);
		return ERROR;
	}
	
	if (pid == CHILD_PROCCESS){
		err = writerChild(fd);
		if (err == ERROR)
			return ERROR;
		return SUCCESS;
	}

	pid = wait(NULL);
	if (pid == ERROR){
		perror("Error in function wait");
		return ERROR;
	}
	return 0;
}
