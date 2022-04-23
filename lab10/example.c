#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]){
        int arg = *argv[1] - 48;
//      printf("%d\n", arg);
        if (arg == 0) {
                int *pointer = NULL;
                *pointer = 1;
        }
        if (arg == 1) {
                int k = 1/0;
        }
        if (arg == 2) {
                kill(getpid(), SIGKILL);
        }
        if (arg == 3) {
                kill(getpid(), SIGSTOP);
        }
        if (arg == 4) {
                return 0;
        }
}
