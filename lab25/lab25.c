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

int closeFD(int fd){
	int err = close(fd);
	if (err == ERROR){
		perror("close error");
		return ERROR;
	}
	return SUCCESS;
}

int closeBothFD(int *fd){
	int err = closeFD(fd[FOR_READ]);
	if (err == ERROR){
		return ERROR;
	}
	err = closeFD(fd[FOR_WRITE]);
	if (err == ERROR){
		return ERROR;
	}
	
	return SUCCESS;
}

int readFromPipe(int *fd){
	char c;
	int err, length;
	do {
		length = read(fd[FOR_READ], &c, 1);
		if (length == ERROR){
			perror("read error");
			return ERROR;
		}
		if (length == 0) break;
		
		c = toupper(c);

		err = write(STDOUT_FILENO, &c, 1);
		if (err == ERROR){
			perror("write error");
			return ERROR;
		}
	} while (length > 0);
	
	return SUCCESS;
}

int writeInPipe(int *fd){
	char c;
	int err, length;	
	do{
		length = read(STDIN_FILENO, &c, 1);
		if (length == ERROR){
			perror("read error");
			return ERROR;
		}
		if (length == 0) break;
		err = write(fd[FOR_WRITE], &c, 1);
		if (err == ERROR){
			perror("write error");
			return ERROR;
		}
	} while (length > 0);
	
	return SUCCESS;
}

int readerProc(int *fd){
	int err;
	
	err = closeFD(fd[FOR_WRITE]);
	if (err == ERROR){
		return ERROR;
	}
	err = readFromPipe(fd);
	if (err == ERROR){
		return ERROR;
	}
	
	err = closeFD(fd[FOR_READ]);
	if (err == ERROR){
		return ERROR;
	}
	return SUCCESS;
}

int writerProc(int *fd){
	int err;
	
	err = closeFD(fd[FOR_READ]);
	if (err == ERROR){
		return ERROR;
	}
	
	err = writeInPipe(fd);
	if (err == ERROR){
		return ERROR;
	}
	
	err = closeFD(fd[FOR_WRITE]);
	if (err == ERROR){
		return ERROR;
	}
	
	return SUCCESS;
}

int createChildProc(int *fd){
	pid_t pid = fork();
	if (pid == ERROR){
		closeBothFD(fd);
		perror("fork error");
		return ERROR;
	}
	return pid;
}

int main(){
	int fd[2];
	int err;
	err = pipe(fd);
	if (err == ERROR){
		perror("pipe error");
		return ERROR;
	}
	
	pid_t pid = createChildProc(fd);
	if (pid == ERROR) return ERROR;
	
	if (pid == CHILD_PROCCESS){
		err = writerProc(fd);
		if (err == ERROR)
			return ERROR;
		return SUCCESS;
	}
	
	pid = createChildProc(fd);
	if (pid == ERROR) return ERROR;
	
	if (pid == CHILD_PROCCESS){
		err = readerProc(fd);
		if (err == ERROR)
			return ERROR;
		return SUCCESS;
	}
	
	err = closeBothFD(fd);
	if (err == ERROR) return ERROR;
	int stat;
	int childNum = 0;
	while (wait(&stat) > 0) {
		childNum++;
	}
	if (childNum != 2){
		perror("wait error");
		return ERROR;
	}

	return 0;
}
