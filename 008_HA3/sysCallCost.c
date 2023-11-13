#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

double findSystemCallCost()
{
    //For storing the file descriptors of the pipe
    int pipefds[2];

    //Creating a buffer; not sure if function would still perform
    //functionality if given null instead of this buffer
	char buffer[1];

    //Creating a pipe, returning file descriptors, and error check
	if(pipe(pipefds) ==-1)  { perror("pipe");   exit(EXIT_FAILURE);}
    close(pipefds[1]);

    //Instantiation of an object of timeval structure defined in sys/time.h
    struct timeval current_time;

    //Profiling start time
    gettimeofday(&current_time, NULL);

    //Variables for tracking start and end time
    double start_time = current_time.tv_usec;
    double end_time = 0;

    //Running a loop of system call
    int loop = 100000;
	for (int i=0; i<loop; i++)
    {
        int j = read(pipefds[0], buffer, 0);
	}

    //Getting end time
    gettimeofday(&current_time, NULL);
    end_time = current_time.tv_usec;

    //Closing read of pipe
    close(pipefds[0]);

    //Printing log
    printf("Start time is '%d'\n", (int)start_time);
    printf("End   time is '%d' in %d number of syscalls \n", (int)end_time, loop);
    printf("Time it took is '%f'\n", ((end_time-start_time)/(loop)));
};

int main()
{
	findSystemCallCost();
	return EXIT_SUCCESS;

}