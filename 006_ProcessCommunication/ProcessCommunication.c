#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void) {

int pipefds[2];
char buffer[5];

if(pipe(pipefds) == -1) {
perror("pipe");
exit(EXIT_FAILURE);
}

char *pin = "4128\0";

int rc = fork();

if(rc == 0)
{
printf("Hello! I am child with id %d and pid %d \n", rc, getpid());
printf("Writing PIN to pipe...\n");
write(pipefds[1], pin, 5);
printf("Done.\n\n");
}
else
{
wait(rc);
printf("Hello! I am father with id %d and pid %d \n", rc, getpid());
printf("Reading PIN from pipe...\n");
read(pipefds[0], buffer, 5);
printf("Done.\n\n");
printf("PIN from pipe: %s\n", buffer);
}
FILE *fp;
fp = fopen("sample.txt","a");
fprintf(fp, "File descriptor \n ");
fclose(fp);
return EXIT_SUCCESS;
}