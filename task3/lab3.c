#include<stdio.h>
#include<errno.h>
#include <unistd.h>
#include <sys/types.h>

#define SETUID_ERROR 2
#define ERROR -1
#define FOPEN_ERROR 3
#define FCLOSE_ERROR 4
#define SUCCESS 0

void printID(){
	uid_t uid = getuid();
	uid_t euid = geteuid();
	
	printf("uid == %d, euid == %d\n", uid, euid);
}

int openFile(){

        FILE * file = fopen("myfile", "r+w");
        if (file == NULL){
                perror("Unable to open file");
                return FOPEN_ERROR;
        }
	printf("File opened succesfully\n");
                
	int close = fclose(file);
                
        if (close == EOF) {
           	perror("Unable to close file");
              	return FCLOSE_ERROR;
        }
        
        return SUCCESS;
}

int main(){

	printID();
	
	int res = openFile();
	if (res == FCLOSE_ERROR){
		return FCLOSE_ERROR;
	}
	
        int s_uid = setuid(getuid());
        if (s_uid == ERROR){
                perror("Error in function setuid");
                return(SETUID_ERROR);
        }
        
	printID();
	
	res = openFile();
	if (res == FCLOSE_ERROR){
		return FCLOSE_ERROR;
	}
	
        return res;
}
