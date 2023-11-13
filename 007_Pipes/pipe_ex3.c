#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(void)
{
	int pipefds[2];
	char*pin;
	char buffer[10];

	if(pipe(pipefds) ==-1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	pid_t pid = fork();
	if (pid ==0)
	{ // in child process
		sleep(1); // intentional delay
		pin= "This is a test\0"; // PIN to send
		close(pipefds[0]); // close read fd
		
		for(int i =0; i<14; i++)
		{
			int j = write(pipefds[1], pin+i,1); // write PIN to pipe
			sleep(1);
		}
		sleep(5);
		close(pipefds[1]);

		exit(EXIT_SUCCESS);
	}
	if(pid>0){ // in main process
		
		close(pipefds[1]); // close write fd
		for (int i=0;i<20;i++)
		{
			int j = read(pipefds[0], buffer,1); // read PIN from pipe
			printf("Parent received PIN '%s', chars read %d\n", buffer, j);
		}

		close(pipefds[0]); // close read fd
		wait(NULL); // wait for child process to finish

	}
	return EXIT_SUCCESS;

}
