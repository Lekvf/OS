#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define ERROR -1
#define TIME_ERROR 1
#define SETENV_MEMORY_ALLOCATE_ERROR 2
#define CTIME_ERROR 3
#define LOCALTIME_ERROR 4

extern char *tzname[];

int main()
{
    time_t now;
    struct tm *sp;
    char *Ctime;

    int err;

    err = time(&now);
    if (err == ERROR){
            perror("Error in function time");
            exit(TIME_ERROR);
    }

    err = setenv("TZ", "PST8PDT", 1);
    if (err == ERROR){
            perror("Error in function setenv");
            exit(SETENV_MEMORY_ALLOCATE_ERROR);
    }

    Ctime = ctime(&now);
    if (!Ctime){
            perror("Error in function ctime");
            exit(CTIME_ERROR);
    }
    printf("%s", Ctime);

    sp = localtime(&now);
    if (!sp){
            perror("Error in function localtime");
            exit(LOCALTIME_ERROR);
    }

    printf("%02d/%d/%02d %d:%02d:%02d %s\n",
            sp->tm_mon + 1, sp->tm_mday,
            sp->tm_year+1900, sp->tm_hour,
            sp->tm_min, sp->tm_sec, tzname[sp->tm_isdst]);

    return(0);
}
~           
