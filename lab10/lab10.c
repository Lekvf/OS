#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#define ERROR -1
#define FORK_ERROR 2
#define EXEC_ERROR 3
#define WAIT_ERROR 4
#define SUCCESSFUL_COMPLETION 0
#define NO_ARGUMENTS 1
#define TRUE 1
#define I_AM_CHILD_PROCCESS 0

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
		int err = execvp(argv[1], &argv[1]);
		if (err == ERROR){
			perror("error in function exec");
			return EXEC_ERROR;
		}
	}
	
	int statloc;
	pid = wait(&statloc);

	if (pid == ERROR){
		perror("Error in function wait");
		return WAIT_ERROR;
	}
	
	if (WIFEXITED(statloc) == TRUE) {
		printf("\nPID of child proccess is %d\n", pid);
		printf("Finished with %d status\n", WEXITSTATUS(statloc) );
	}

	return 0;
}
