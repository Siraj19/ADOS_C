#include <stdio.h>
#include <stdlib.h>

void max(int*, int*);

int main(int argc, char* argv[])
{
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);
    max(&num1, &num2);
    printf("The larger number is %d \n", num1);
    return 0;
}

void max(int* num1, int* num2)
{
    if(*num2 > *num1)
    {
        *num1 = *num2;
    }
}