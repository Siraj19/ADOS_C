#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>


int main(int argc, char *argv[])
{
    for (int i = 0; i<argc; i++)
    {
        printf("%s\n", argv[i]);
    }
    return 0;
}