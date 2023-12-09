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

/*void charArr2Args(char** argArr, char* argCharArray, int size)
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
}*/

int findNumOfCommands(const char* argCharArray, const int size)
{
    int count = 1;
    for(int i = 1; i<size; i++)
    {
        if(*(argCharArray+i)=='&'){count++;}
    }
    return count;
}


void charArr2MultiCommandArgs(char*** argArr, char* argCharArray, int size, int commandNum)
{
    int localCommandCount = 0;
    int count = 1;
    if(argArr != NULL)
    {
        free(argArr);
    }
    argArr = malloc((commandNum+1)*sizeof(char**));
    *(argArr+commandNum)=NULL;                      //Termination on NULL for safety 
    
    char* tempPaste = argCharArray;
    for(int i = 0; i<size; i++)
    {
        if(*(argCharArray+i)=='\0'){
            count++;
        }
        if(*(argCharArray+i)=='&'){
            *(argArr+localCommandCount) = malloc((count)*sizeof(char*));
            **(argArr+localCommandCount) = tempPaste;
            *(*(argArr+localCommandCount)+count-1) = NULL;   //Termination on NULL for safety   
            tempPaste = argCharArray+i+2;
            localCommandCount++;
            count = 0;
        }
        //if(localCommandCount>commandNum+1){                 //Termination on safety case
        //    break;
        //}
    }
    *(argArr+localCommandCount) = malloc((count+1)*sizeof(char*));
    **(argArr+localCommandCount) = tempPaste;
    *(*(argArr+localCommandCount)+count-1) = NULL;

    localCommandCount = 0;
    count = 1;
    for (int i = 1; i<size; i++)
    {
        if(*(argCharArray+i)=='\0'){
            if(*(argCharArray+i+1)!='&')
            {
             *(*(argArr+localCommandCount)+count) = argCharArray+i+1;  
             *(argCharArray+i)=NULL; 
             count++;
            }
             
        }
        if(*(argCharArray+i)=='&')
            {
            *(argCharArray+i)='\0'; 
             localCommandCount++;  
             count=0;
            }

    }
    /*
    //Test print
     printf("I went through here \n");
    for(int i = 0; i< commandNum; i++)
    {
        if(*(argArr+i)== NULL)
        {
            break;
        }
        printf("So there are commands \n");
        for(int j = 0; *(*(argArr+i)+j)!= NULL; j++)
        {
            printf("So there are arguments \n");
            printf("%s \n", *(*(argArr+i)+j));
        }
        printf("\n");
    } */

}

int main() {
    char *arg = NULL;    // Initialize the pointer to NULL
    char **argArr = NULL;
    char *path = malloc(100*sizeof(char));
    char **pathArr = NULL;
    char ***cmdsArgArr = NULL;

    size_t bufferSize;      // Size of string
    size_t charNum;
    size_t numArgs;
    size_t numPaths;
    size_t numCommands;
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
            ///////
            numCommands = findNumOfCommands(arg, charNum);
            charArr2MultiCommandArgs(cmdsArgArr, arg, charNum, numCommands);
            int rc = fork();
            if(rc == 0)
            {
                for(int i=0; i<numPaths; i++)
                {
                    print("+ \n");
                    for(int j = 0; j < numCommands; j++)
                    {
                        print("++ \n");
                        int rrc = fork();
                        if(rrc == 0)
                        {
                            printf("This is rrc : %s \n", *(cmdsArgArr+j)); 
                            execvp(strcpy(pathArr[i], **(cmdsArgArr+j)), *(cmdsArgArr+j));
                            //printf("%s \n", *(cmdsArgArr+j));     
                        }
                        
                    }
                    
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
