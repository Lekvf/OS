#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <signal.h>

#define ERROR -1
#define BACKLOG 1
#define DEFAULT 0
#define SUCCESS 0
#define BUF_SIZE 256
#define SOCKET_PATH "socket"

int unlinkName(char *socketName);

void sighandler(int sig){
        unlinkName(SOCKET_PATH);
        printf("\n");
        exit(0);
}

int readMessage(int clientFD){
        int length, err;
        char buf[BUF_SIZE];
        do {
                length = read(clientFD, buf, BUF_SIZE);
                if (length == ERROR){
                        perror("read error");
                        return ERROR;
                }
                if (length == 0) break;

                for (int i = 0; i < length; i++){
                        buf[i] = toupper(buf[i]);
                }
                err = write(STDOUT_FILENO, buf, length);
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

int connectWithClient(int socketFD, struct sockaddr_un *addr){
        int err = bind(socketFD, (struct sockaddr *) addr, sizeof(struct sockaddr_un));
        if (err == ERROR) {
                perror("bind error");
                return ERROR;
        }

        err = listen(socketFD, BACKLOG);
        if (err == ERROR) {
                perror("listen error");
                return ERROR;
        }

        int clientFD = accept(socketFD, NULL, NULL);
        if (clientFD == ERROR) {
                perror("accept error");
                return ERROR;
        }
        return clientFD;
}

int unlinkName(char *socketName){
        int err = unlink(socketName);
        if (err == ERROR){
                perror("unlink error");
                return ERROR;
        }
}
int main(int argc, char *argv[]) {

        struct sigaction act;
        memset(&act, 0, sizeof(act));
        act.sa_handler = sighandler;
        int sigactionResult = sigaction(SIGINT, &act, 0);

        if (sigactionResult == ERROR){
                perror("sigaction error");
                return ERROR;
        }

        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        int socketFD = socket(AF_UNIX, SOCK_STREAM, DEFAULT);
        if (socketFD == ERROR) {
                unlinkName(SOCKET_PATH);
                perror("socket error");
                return ERROR;
        }
        addr.sun_family = AF_UNIX;

        strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

        int clientFD = connectWithClient(socketFD, &addr);
        if (clientFD == ERROR){
                unlinkName(SOCKET_PATH);
                closeFD(socketFD);
                return ERROR;
        }

        int err = closeFD(socketFD);
        if (err == ERROR){
                unlinkName(SOCKET_PATH);
                closeFD(clientFD);
                return ERROR;
        }

        err = readMessage(clientFD);
        if (err == ERROR){
                unlinkName(SOCKET_PATH);
                err = closeFD(clientFD);
                return ERROR;
        }
        err = unlinkName(SOCKET_PATH);
        if (err == ERROR){
                perror("unlink error");
                return ERROR;
        }

        return 0;
}
