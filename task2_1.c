#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define TIME_ERROR -1
#define SETENV_MEMORY_ALLOCATE_ERROR -1

extern char *tzname[];

int main()
{
    time_t now;
    struct tm *sp;
    char *Ctime;

    int err = 1;

    err = time(&now);
    if (err == TIME_ERROR){
            perror("Error in function time");
            exit(1);
    }

    err = setenv("TZ", "PST8PDT", 1);
    if (err == SETENV_MEMORY_ALLOCATE_ERROR){
            perror("Error in function setenv");
            exit(1);
    }

    Ctime = ctime(&now);
    if (!Ctime){
            perror("Error in function ctime");
            exit(1);
    }
    printf("%s", Ctime);

    sp = localtime(&now);
    if (!sp){
            perror("Error in function localtime");
            exit(1);
    }

    printf("%02d/%d/%02d %d:%02d:%02d %s\n",
            sp->tm_mon + 1, sp->tm_mday,
            sp->tm_year+1900, sp->tm_hour,
            sp->tm_min, sp->tm_sec, tzname[sp->tm_isdst]);

    return(0);
}
