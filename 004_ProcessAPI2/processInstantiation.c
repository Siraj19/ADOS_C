#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
printf("Start (pid:%d)\n\n", (int) getpid());

int rc = fork();
if (rc < 0)
{
    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(1);
} 
else if (rc == 0) 
{ 
    // child (new process)
        wait(rc);
        printf("hello, I am child (pid:%d)\n", (int) getpid());
} 
else {
    // parent goes down this path (main)
        printf("hello, I am parent of %d (pid:%d)\n", rc, (int) getpid());
    }
    printf("Bye Bye! \n\n");
return 0;
}