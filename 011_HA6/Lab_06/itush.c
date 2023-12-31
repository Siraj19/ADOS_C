#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int findNumOfArgs(const char* argCharArray, const int size)
{
    int count = 1;
    for(int i = 1; i<size; i++)
    {
        if(*(argCharArray+i)==' '){count++;}
    }
    return count;
}

void charArr2Args(char** argArr, char* argCharArray, int size)
{
    *argArr = argCharArray;
    int count = 1;
    for(int i = 1; i<size; i++)
    {
        if(*(argCharArray+i)==' ')
        {
            *(argCharArray+i) = '\0';
            *(argArr+count) = argCharArray+i+1;
            count++;
        }
    }
    *(argCharArray+size-1) = NULL; 
    *(argArr+count) = NULL; 
}

int main() {
    char *arg = NULL;    // Initialize the pointer to NULL
    char **argArr = NULL;
    char *path = malloc(100*sizeof(char));
    char **pathArr = NULL;
    //char path[100];
    size_t bufferSize;      // Size of string
    size_t charNum;
    size_t numArgs;
    size_t numPaths;
    int errCheck;

    while (1) {
        printf("itush> ");  // Shell name
        charNum = getline(&arg, &bufferSize, stdin);
        // Read a line from stdin file descriptor
        // takes double pointer to the character buffer and pointer to integer size

        if (charNum == -1) {
            perror("getline");
            break;
        }
        //printf("%d\n", charNum); 
        printf("%s\n", arg);                            //Print the arguments
        numArgs = findNumOfArgs(arg, charNum);          //Find the number of arguments
        printf("%d \n", numArgs);                       //Printing number of args for test
        argArr = malloc((numArgs+1)*sizeof(char*));     //Initializing space for arguments pointer array
        charArr2Args(argArr, arg, charNum);             //Giving those arguments address
        
        if(strcmp(argArr[0],"exit\0") == 0)
        {
            free(pathArr);
            free(path);
            exit(0);
        }
        else if(strcmp(argArr[0],"cd\0") == 0)
        {
            if(numArgs != 2){
                printf("Error that cd requires only one argument \n");
            }
            else{
                if(chdir(argArr[1]) != 0){
                    printf("Unsuccessful! \n");
                }
            }
        }
        else if(strcmp(argArr[0],"path\0") == 0)
        {
            free(path);
            strcpy(path, argArr[1]);
            for(int i = 2; i < numArgs; i++)
            {
                strcat(path, " ");
                strcat(path, argArr[i]);
            }
            printf("Paths added are: %s \n", path);
            
            if(pathArr!=NULL)
            {
                free(pathArr);
            }
            numPaths = numArgs-1;
            pathArr = malloc((numPaths+1)*sizeof(char*));
            for(int i = 0; i<numArgs-1; i++)
            {
                *(pathArr+i) = argArr[i+1];
            }
            *(pathArr+numArgs-1) = NULL;
        }
        else{
            int rc = fork();
            if(rc == 0)
            {
                for(int i=0; i<numPaths; i++)
                {
                    execvp(strcpy(pathArr[i],argArr[0]), argArr);
                    printf("%s \n", argArr);
                }
                printf("The command does not exist. \n");
            }
            else if (rc == -1)
            {
                printf("Forking failed \n");
            }
            else
            {
                wait();
            }    
        }
        

        free(argArr);
        free(arg);
        arg = NULL;  // Reset the pointer to NULL for the next input
    }

    return 0;
}
