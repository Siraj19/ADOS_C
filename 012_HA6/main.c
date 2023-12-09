#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>
#include <stdio.h>
#include "tlb.c"

int main(int argc, char *argv[])
{

    /*if(argc == 3)
    {
        findAccessCost(atoi(argv[1]), atoi(argv[2]));
    }
    else
    {
        return EXIT_FAILURE;
    }*/
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    //How many times a loop within the function should iterate
    //Which means number of jumps(or pages in this sense) * number of trials
    int iteration = 10000000;   //10 million
    for(int i = 2; i<=1<<20; i=i<<1)
    {
        printf("us took with %d pages is %f \n", i, findAccessCost(i, (iteration/i > 100) ? iteration/i  : 100));
    }
    

    return EXIT_SUCCESS;
}