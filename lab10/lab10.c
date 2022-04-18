#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>

#define ERROR -1
#define FORK_ERROR 2
#define EXEC_ERROR 3
#define WAIT_ERROR 4
#define SUCCESSFUL_COMPLETION 0
#define NO_ARGUMENTS 1
#define TRUE 1
#define I_AM_CHILD_PROCCESS 0

void childEndingStatus(int statloc);

int main(int argc, char *argv[]) {

	if (argc == NO_ARGUMENTS){
		printf("Nothing to do: have no arguments\n");
		return SUCCESSFUL_COMPLETION;
	}

	pid_t pid = fork();
    
	if (pid == ERROR){
		perror("Error in function fork");
		return FORK_ERROR;
	}
	
	if (pid == I_AM_CHILD_PROCCESS){
		execvp(argv[1], &argv[1]);
		perror("Error in function exec");
		return EXEC_ERROR;
	}
	
	int statloc;
	pid = wait(&statloc);

	if (pid == ERROR){
		perror("Error in function wait");
		return WAIT_ERROR;
	}
	
	printf("\nChild proccess with PID %d ", pid);
	
	childEndingStatus(statloc);
	
	return 0;
}


void childEndingStatus(int statloc){
	int signal;
	if (WIFEXITED(statloc) == TRUE) {
		signal = WEXITSTATUS(statloc);
		printf("exited with %d status\n", signal);
	}

	if (WIFSIGNALED(statloc) == TRUE){
		signal = WTERMSIG(statloc);
		printf("killed by signal %d\n", signal);
		psignal(signal, "reason");
	}

	if (WIFSTOPPED(statloc) == TRUE){
		signal = WSTOPSIG(statloc);
		printf("stopped by signal %d\n", signal);
		psignal(signal, "reason");
	}

	if (WIFCONTINUED(statloc) == TRUE){
		printf("STILL WORKED!!!\nKILL IT!!\n");
	}

}
