#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

int main()
{
	int pipefds[2];
	char*pin;
	char buffer[10];
    //Instantiation of an object of timeval structure defined in sys/time.h
    struct timeval time;
	cpu_set_t set;

	if(pipe(pipefds) ==-1) { perror("pipe"); exit(EXIT_FAILURE);}
	CPU_ZERO(&set);
	pid_t pid = fork();
	
    if (pid == 0)
	{
		CPU_SET(0, &set);
		// in child process
        sleep(1);
        // PIN to send
		pin = "This is a test\0";
		int childTime[14];
        // close read fd
		close(pipefds[0]);
		
		for(int i = 0; i<14; i++)
		{
			int j = write(pipefds[1], pin+i,1); // write PIN to pipe
            gettimeofday(&time, NULL);
			childTime[i]=time.tv_usec;
			sleep(1);
			//printf("Child time is %d\n", time.tv_usec);
            //printf("Child sent PIN '%c', chars write %d\n", pin[i], j);
		}
		for(int i = 0; i<14; i++)
		{
			printf("ChildTime[%d] -> %d \n", i, childTime[i]);
		}
		printf("\n \n \n");
		sleep(5);
		close(pipefds[1]);
		exit(EXIT_SUCCESS);
	}

    else
    {
		CPU_SET(0, &set);
        // in main process	
		close(pipefds[1]);  // close write fd
		int parentTime[14];

		for (int i=0;i<14;i++)
		{
			int j = read(pipefds[0], buffer,1); // read PIN from pipe
            gettimeofday(&time, NULL);
			parentTime[i]=time.tv_usec;
            //printf("Parent time is %d\n", time.tv_usec);
			//printf("Parent received PIN '%s', chars read %d\n", buffer, j);
		}

		close(pipefds[0]); // close read fd
		wait(NULL); // wait for child process to finish

		for(int i = 0; i<14; i++)
		{
			printf("ParentTime[%d] -> %d \n", i, parentTime[i]);
		}

	}

	return EXIT_SUCCESS;
}