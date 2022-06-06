#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <signal.h>

#define ERROR -1
#define SUCCESS 0
#define BUF_SIZE 256

void sighandler(int sig) {
    fprintf(stderr, "Server unreachable\n");
}

int writeMessage(int socketFD){
	char buf[BUF_SIZE];
	int err, length;	
	do{
		length = read(STDIN_FILENO, buf, BUF_SIZE);
		if (length == ERROR){
			perror("read error");
			return ERROR;
		}
		if (length == 0) break;
		
		err = write(socketFD, buf, length);
		if (err == ERROR){
			perror("write error");
			return ERROR;
		}
	} while (length > 0);
	
	return SUCCESS;
}

int closeFD(int fd){
	int err = close(fd);
	if (err == ERROR){
		perror("close error");
		return ERROR;
	}
	return SUCCESS;
}

int main(int argc, char *argv[]) {
	char *socket_path = "socket";

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = sighandler;
	int sigactionResult = sigaction(SIGPIPE, &act, 0);
	
	if (sigactionResult == ERROR){
		perror("sigaction error");
		return ERROR;
	}
    
	int socketFD = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socketFD == ERROR) {
		perror("socket error");
		return ERROR;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;

	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
	
	int err = connect(socketFD, (struct sockaddr *) &addr, sizeof(addr));
	if (err == ERROR) {
		perror("connect error");
		return ERROR;
	}
	
	err = writeMessage(socketFD);
	if (err == ERROR){
		closeFD(socketFD);
		return ERROR;
	}
	
	err = closeFD(socketFD);
	if (err == ERROR)
		return ERROR;
		
	return 0;
}
