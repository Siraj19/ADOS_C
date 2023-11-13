#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

//Created this function using chatgpt
void initializeBuffer(char **buffer, int size) {
    // Seed the random number generator with the current time
    srand(time(NULL));

    // Allocate memory for the buffer
    *buffer = (char *)malloc(size * sizeof(char));

    if (*buffer == NULL) {
        // Handle memory allocation error
        perror("Memory allocation failed");
        exit(1);
    }

    // Fill the buffer with random ASCII values
    for (int i = 0; i < size-1; i++) {
        (*buffer)[i] = rand() % 128; // ASCII values range from 0 to 127
    }
    (*buffer)[size-1] = 0;
}

int main(void)
{
    //For storing the file descriptors of the pipe
    int pipefds[2];

    //Creating a buffers for reading and writing
    int loop = 10;
	//char buffer[loop];

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

    int rc = fork();

    if (rc == 0)
    {
        close(pipefds[0]);
        char *bufferTX;
        initializeBuffer(&bufferTX, loop);
        //printf(bufferTX);
        for(int i = 0; i < loop; i++)
        {
            gettimeofday(&current_time, NULL);
            start_time = current_time.tv_usec;
            int j = write(pipefds[1], bufferTX, 1);
            printf("write status %d \n", (int)j);
            //printf("Start time is : %d and character is %s \n ", (int)start_time, bufferTX+i);
            sleep(1);
        }
         free(bufferTX);
         //Closing read of pipe
         close(pipefds[1]);
    }
    else
    {
        close(pipefds[1]);
        char bufferRX[loop];
        strcpy(bufferRX, " ");
        sleep(1);
        for(int i = 0; i < loop; i++)
        {
            read(pipefds[0], bufferRX, 1);
            gettimeofday(&current_time, NULL);
            end_time = current_time.tv_usec;
            printf("End time is : %d and character read is %s \n", (int)end_time, bufferRX);
        }
        //Closing read of pipe
        close(pipefds[0]);
        wait(NULL);
    }

	return EXIT_SUCCESS;
}