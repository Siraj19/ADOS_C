#include <stdio.h>
#include <stdlib.h>

void function()
{
    printf("Hello \n");
}

int main(int argc, char *argv[])
{
printf("location of code : %p\n", main);
printf("location of heap : %p\n", malloc(100e6));
int x = 3;
printf("location of stack: %p\n", &x);

printf("This is function with start address: %d", function);
void* funcPointer = function;
funcPointer;
return x;
}