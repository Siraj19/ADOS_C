#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
printf("hello world (pid:%d)\n", (int) getpid());

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
    for(int i = 0; i<10; i++)
    {
        printf("hello, I am child (pid:%d)\n", (int) getpid());
    }
} 
else {
    // parent goes down this path (main)
    for(int j = 0; j<10; j++)
    {
        printf("hello, I am parent of %d (pid:%d)\n", rc, (int) getpid());
    }
    }
    printf("Bye Bye! \n");
return 0;
}