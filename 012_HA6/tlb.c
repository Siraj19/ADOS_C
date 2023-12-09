#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>


double findAccessCost(int numberOfPages, int numberOfTrials)
{
    //Instantiation of an object of timeval structure defined in sys/time.h
    struct timeval current_time;

    //Profiling start time
    gettimeofday(&current_time, NULL);

    //Variables for tracking start and end time
    double start_time_us = current_time.tv_usec;
    double end_time_us = 0;
    int start_time_sec = current_time.tv_sec;
    int end_time_sec   = 0;
    
    long PAGESIZE = sysconf(_SC_PAGE_SIZE);
    int jump = PAGESIZE / sizeof(int);
    int* a = malloc(numberOfPages*jump*sizeof(int));

    for(int s = 0; s < numberOfTrials; s++)
    {
      for (int i = 0; i < numberOfPages * jump; i += jump)
      {
        a[i] += 1;
        }  
    }

    //Getting end time
    gettimeofday(&current_time, NULL);
    end_time_us = current_time.tv_usec;
    end_time_sec = current_time.tv_sec;
    free(a);
    //Printing log
    //printf("Start time is %d seconds '%d' microseconds \n", (int)start_time_sec, (int)start_time_us);
    //printf("End   time is %d seconds '%d' microseconds \n", (int)end_time_sec, (int)end_time_us);
    //printf("Microseconds time it took is '%f'\n", (1000000*(((end_time_us-start_time_us)+1000000*(end_time_sec-start_time_sec))/(numberOfPages * jump))/numberOfTrials));
    
    return (1000000*(((end_time_us-start_time_us)+1000000*(end_time_sec-start_time_sec))/(numberOfPages * jump))/numberOfTrials);
};