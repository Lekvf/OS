#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#define ERROR -1
#define FORK_ERROR -2
#define SUCCESSFUL_COMPLETION 0
#define NO_ARGUMENTS 1
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
		int err = execvp(argv[1], argv + 1);
		
		if (err == ERROR){
			perror("error in function exec");
			return ERROR;
		}
	}
	
	int statloc;
	pid = wait(&statloc);
	
	if (pid == ERROR){
		perror("Error in function wait");
		return ERROR;
	}
	
	if (WIFEXITED(statloc)) {
		printf("proccess with pid = %d\n finished with status %d\n", pid, WEXITSTATUS(statloc));
	}

	return 0;
}
