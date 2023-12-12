#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>

//Warning that the page size should be larger than your largetextfile
#define PAGE_SIZE 1024*1024*32  //32MB MAX file size allowed
#define BUFFER_SIZE 1024
#define MAX_THREADS 1024
#define MIN_THREADS 1
#define MAX_BLOCKSIZE PAGE_SIZE
#define MIN_BLOCKSIZE 1

struct histogramBinStringInfo
{
    int numberOfBins;
    int * numberOfCharatersInBins;
    char ** charactersInBins;
};

struct largeTextStringInfo
{
    int size;
    char * largeTextString;
};

struct largeTextHistogramBinStringInfo
{
    int numberOfBins;
    int * numberOfCharatersInBins;
};

struct userConstraintInfo
{
    int numberOfThreads;
    int blockSize;
};

struct threadArgInfo
{
    int threadID;
    struct histogramBinStringInfo * hBSI;
    struct largeTextStringInfo * lTSI;
    struct userConstraintInfo * uCI;
};

struct threadReturnInfo
{
    int threadID;
    int * numberOfCharatersInBins;
};

struct threadArgInfo * packthreadArgInfo(struct histogramBinStringInfo * hBSI, struct largeTextStringInfo * lTSI, struct userConstraintInfo * uCI, int threadID)
{
    struct threadArgInfo * tAI = (struct threadArgInfo *)malloc(sizeof(struct threadArgInfo));
    tAI->threadID = threadID;
    tAI->hBSI = hBSI;
    tAI->lTSI = lTSI;
    tAI->uCI = uCI;
}

struct userConstraintInfo * packUserConstraintInfo(const int numberOfThreads, const int blockSize)
{
    struct userConstraintInfo * uCI = (struct userConstraintInfo *)malloc(sizeof(struct userConstraintInfo));
    uCI->numberOfThreads = numberOfThreads;
    uCI->blockSize = blockSize;
}

struct largeTextStringInfo * findLargeTextStringInfo(char * str)
{
    struct largeTextStringInfo * lTSI = (struct largeTextStringInfo *)malloc(sizeof(struct largeTextStringInfo));
    //Size of string should not be computed again and again. Hence there should be a structure to keep it in record for handy
    lTSI->size = strlen(str);
    lTSI->largeTextString = str;
    return lTSI;
}

int findCharFrequencyInString(const char character, const char * str)
{
    int frequency = 0;
    for(int i = 0; *(str+i)!='\0'; i++) if(*(str+i)==character) frequency++;
    return frequency;
}

struct histogramBinStringInfo * findHistogramBinStringInfo(const char * str)
{
    struct histogramBinStringInfo * hBSI = (struct histogramBinStringInfo *)malloc(sizeof(struct histogramBinStringInfo));
    //Number of bins is equal to number of lines plus one for other
    hBSI->numberOfBins = findCharFrequencyInString('\n', str)+2;
    hBSI->numberOfCharatersInBins = calloc(hBSI->numberOfBins, sizeof(int));
    //Now finding number of characters in all bins
    int numberOfCharacters = 1;
    int bin = 0;
    for(int i = 0; *(str+i)!= '\0'; i++)
    {
        if(*(str+i)==',') numberOfCharacters++;
        if(*(str+i)=='\n'){
            hBSI->numberOfCharatersInBins[bin] = numberOfCharacters;
            bin++;
            numberOfCharacters = 1;
        }
    }
    hBSI->numberOfCharatersInBins[bin] = numberOfCharacters;
    
    //Allocating space for 2d array or double pointer for storing characters of each bin
    hBSI->charactersInBins = (char **)malloc(sizeof(char *)*(hBSI->numberOfBins-1));
    for(bin = 0; bin<(hBSI->numberOfBins); bin++)
    {
        hBSI->charactersInBins[bin]=malloc(sizeof(char)*hBSI->numberOfCharatersInBins[bin]);
    }
    //Now fetching characters for each bin in 2d array or double pointer
    bin = 0;
    numberOfCharacters = 0;
    for(int i = 0; *(str+i)!= '\0'; i++)
    {
        if(*(str+i)==',') numberOfCharacters++;
        else if (*(str+i)=='\n') 
        {
            bin++;
            numberOfCharacters = 0;
        }
        else hBSI->charactersInBins[bin][numberOfCharacters] = *(str+i);
    }
    return hBSI;
}

void printLargeTextHistogramBinStringInfo(struct largeTextHistogramBinStringInfo * lTHBSI)
{
    printf("Number of bins: %d \n", lTHBSI->numberOfBins);
    for(int bin = 0; bin<lTHBSI->numberOfBins; bin++)
    {
        printf("Number of characters in %d bin is %d \n", bin, lTHBSI->numberOfCharatersInBins[bin]);
    }
}

void printHistogramBinStringInfo(struct histogramBinStringInfo * hBSI)
{
    printf("Number of bins: %d \n", hBSI->numberOfBins);
    for(int bin = 0; bin<hBSI->numberOfBins-1; bin++)
    {
        printf("Number of characters in %d bin is %d. \n", bin, hBSI->numberOfCharatersInBins[bin]);
        printf("Characters are : ");
        for(int c = 0; c < (hBSI->numberOfCharatersInBins[bin]); c++)
        {
            printf("%c ", hBSI->charactersInBins[bin][c]);
        }
        printf("\n");
    }
}

void * sectionedPartioningWorkerFunction(void * arg) 
{
    const struct threadArgInfo * tAI = (struct threadArgInfo *)arg;
    struct threadReturnInfo * tRI = (struct threadReturnInfo *)malloc(sizeof(struct threadReturnInfo));
    tRI->threadID = tAI->threadID;
    tRI->numberOfCharatersInBins = calloc(sizeof(int), tAI->hBSI->numberOfBins-1);
    //printf("Hello, I am thread %d \n", tAI->threadID);
    //Return with 0 count instead of doing segmentation fault due to limitation of large text string
    if ((tAI->threadID)*(tAI->lTSI->size/tAI->uCI->numberOfThreads+1)>(tAI->lTSI->size))
    {
        return (void *) tRI;
    }

    for(int i = (tAI->threadID)*(tAI->lTSI->size/tAI->uCI->numberOfThreads+1); i<(tAI->threadID+1)*(tAI->lTSI->size/tAI->uCI->numberOfThreads+1); i++)
    {
        if(tAI->lTSI->largeTextString[i] == '\0')
        {
            return (void *) tRI;
        }

        for(int b = 0; b < (tAI->hBSI->numberOfBins-1); b++)
        {
            for(int c = 0; c < (tAI->hBSI->numberOfCharatersInBins[b]); c++)
            {
                if(tAI->lTSI->largeTextString[i] == tAI->hBSI->charactersInBins[b][c])
                {
                    tRI->numberOfCharatersInBins[b]++; 
                }
                
            }
        }
    }

    return (void *) tRI;
}

void * interleavedPartioningWorkerFunction(void * arg) 
{
    const struct threadArgInfo * tAI = (struct threadArgInfo *)arg;
    struct threadReturnInfo * tRI = (struct threadReturnInfo *)malloc(sizeof(struct threadReturnInfo));
    tRI->threadID = tAI->threadID;
    tRI->numberOfCharatersInBins = calloc(sizeof(int), tAI->hBSI->numberOfBins-1);
    //printf("Hello, I am thread %d \n", tAI->threadID);
    //Return with 0 count instead of doing segmentation fault due to limitation of large text string
    if ((tAI->threadID)*(tAI->uCI->blockSize)>(tAI->lTSI->size))
    {
        return (void *) tRI;
    }

    for(int sb = 0; (sb*(tAI->uCI->blockSize*tAI->uCI->numberOfThreads)+tAI->uCI->blockSize*tAI->threadID)<(tAI->lTSI->size); sb++)
    {
        for(int i = (sb*(tAI->uCI->blockSize*tAI->uCI->numberOfThreads)+tAI->uCI->blockSize*tAI->threadID); i<(sb*(tAI->uCI->blockSize*tAI->uCI->numberOfThreads)+tAI->uCI->blockSize*(tAI->threadID+1)); i++)
        {
            if(tAI->lTSI->largeTextString[i] == '\0')
            {
                return (void *) tRI;
            }

            for(int b = 0; b < (tAI->hBSI->numberOfBins-1); b++)
            {
                for(int c = 0; c < (tAI->hBSI->numberOfCharatersInBins[b]); c++)
                {
                    if(tAI->lTSI->largeTextString[i] == tAI->hBSI->charactersInBins[b][c])
                    {
                        tRI->numberOfCharatersInBins[b]++; 
                    }
                }
            }
        }
    }
    return (void *) tRI;
}



int main(int argc, char *argv[]) {
    //Checking if number of input arguments are complete
    if(argc != 6){
        fprintf(stderr, "Number of arguments are %d instead of 4.\n", argc-1);
        exit(EXIT_FAILURE);
    }
    //Assigning pointers to their respective addresses
    const char * pathLargeTextFile = argv[1];
    const char * pathHistogramBinsFile = argv[2];
    //Converting asci string character to their respective equivalent integer
    const int numberOfThreads = atoi(argv[3]);
    const int blockSize = atoi(argv[4]);
    //Assigning pointer to the argument which is dictating
    //Interleaved or Sectioned
    const char * partionSelect = argv[5];
    //Checking if flags are correct
    if(!(*partionSelect == 'i'||*partionSelect == 's'))
    {
        fprintf(stderr, "Partioning flag %s is incorrect.\n", partionSelect);
        exit(EXIT_FAILURE);
    }
    //Checking if number of threads are positive and a valid number
    if((numberOfThreads < MIN_THREADS) || (numberOfThreads > MAX_THREADS))
    {
        fprintf(stderr, "Invalid number of threads %d \n", numberOfThreads);
        exit(EXIT_FAILURE);
    }
    if((blockSize < MIN_BLOCKSIZE) || (blockSize > MAX_BLOCKSIZE))
    {
        fprintf(stderr, "Invalid block size %d \n", blockSize);
        exit(EXIT_FAILURE);
    }
    //Reading input files from the given path & file name
    FILE * largeTextFile = fopen(pathLargeTextFile, "r");
    FILE * histogramBinsFile = fopen(pathHistogramBinsFile, "r");
    //Result would be dumped in this file in case of successful ending
    FILE * resultHistogramBinsFile = NULL;
    //Checks in case of file is not being read.
    if(largeTextFile == NULL){
        fprintf(stderr, "Could not open file %s for reading.\n", pathLargeTextFile);
        exit(EXIT_FAILURE);
    }
    if(histogramBinsFile == NULL){
        fprintf(stderr, "Could not open file %s for reading.\n", pathHistogramBinsFile);
        exit(EXIT_FAILURE);
    }
    printf("Number of threads are: %d \n", numberOfThreads);
    printf("Partioning scheme is: %s \n", (*partionSelect == 's')?"Sectioned":"Interleaved");
    if(*partionSelect == 'i') printf("Block sizes is: %d \n", blockSize);
    //Now profiling time
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    //Variables for tracking start and end time
    long start_time_us = current_time.tv_usec;
    long end_time_us = 0;
    long start_time_sec = current_time.tv_sec;
    long end_time_sec = 0;

    //Reading large text file in buffer then pasting back to largeTextString
    char * buffer = malloc(BUFFER_SIZE);
    char * largeTextString = malloc(PAGE_SIZE);
    while (fgets(buffer, BUFFER_SIZE, largeTextFile) != NULL) {
        strcat(largeTextString, buffer);
    }
    printf("Your large text string size is %d \n", (int)strlen(largeTextString));
    free(buffer);
    //Reading histograms bin file in buffer then pasting back to histogramBinString
    buffer = malloc(BUFFER_SIZE);
    char * histogramBinString = malloc(PAGE_SIZE);
    while (fgets(buffer, BUFFER_SIZE, histogramBinsFile) != NULL) {
        strcat(histogramBinString, buffer);
    }
    printf("Your histogram bin string size is %d \n", (int)strlen(histogramBinString));
    free(buffer);
    printf("\n");
    //Measuring file reading time
    //Time taken to compute results
    gettimeofday(&current_time, NULL);
    end_time_us = current_time.tv_usec;
    end_time_sec = current_time.tv_sec;
    printf("Time taken to read is %f seconds \n", (float)((end_time_us-start_time_us)/1000000.0+(end_time_sec-start_time_sec)));
    gettimeofday(&current_time, NULL);
    start_time_us = current_time.tv_usec;
    start_time_sec = current_time.tv_sec;

    //Creating args for threads
    struct histogramBinStringInfo * hBSI = findHistogramBinStringInfo(histogramBinString);
    struct largeTextStringInfo * lTSI = findLargeTextStringInfo(largeTextString);
    struct userConstraintInfo * uCI = packUserConstraintInfo(numberOfThreads, blockSize);
    struct threadArgInfo ** tAIarr = malloc(sizeof(struct threadArgInfo *)*(uCI->numberOfThreads));
    struct threadReturnInfo ** tRIarr = malloc(sizeof(struct threadReturnInfo *)*(uCI->numberOfThreads));
    struct largeTextHistogramBinStringInfo * lTHBSI = malloc(sizeof(struct largeTextHistogramBinStringInfo));
    //Initializing results
    lTHBSI->numberOfBins = hBSI->numberOfBins;
    lTHBSI->numberOfCharatersInBins = calloc(sizeof(int), lTHBSI->numberOfBins);
    lTHBSI->numberOfCharatersInBins[lTHBSI->numberOfBins-1] = lTSI->size;

    for(int i = 0; i < uCI->numberOfThreads; i++)
    {
        tAIarr[i] = packthreadArgInfo(hBSI, lTSI, uCI, i);
    }
    //Creating threads with functions assigned with partionSelect flag
    pthread_t p[uCI->numberOfThreads];
    
    if(*partionSelect == 'i')for(int i = 0; i < uCI->numberOfThreads; i++)
    {
        pthread_create(&p[i], NULL, (void *)interleavedPartioningWorkerFunction, tAIarr[i]);
    }
    if(*partionSelect == 's')for(int i = 0; i < uCI->numberOfThreads; i++)
    {
        pthread_create(&p[i], NULL, (void *)sectionedPartioningWorkerFunction, tAIarr[i]);
    }

    for(int i = 0; i < uCI->numberOfThreads; i++)
    {
        pthread_join(p[i], (void **) &tRIarr[i]);
        //printf("Thread %d returned back to the main. \n", tRIarr[i]->threadID);
        for(int b = 0; b < lTHBSI->numberOfBins-1; b++)
        {
            lTHBSI->numberOfCharatersInBins[b] += tRIarr[i]->numberOfCharatersInBins[b];
            lTHBSI->numberOfCharatersInBins[lTHBSI->numberOfBins-1] -= tRIarr[i]->numberOfCharatersInBins[b];
        }
    }
    //Time taken to compute results
    gettimeofday(&current_time, NULL);
    end_time_us = current_time.tv_usec;
    end_time_sec = current_time.tv_sec;

    printf("Time taken to compute is %f seconds \n", (float)((end_time_us-start_time_us)/1000000.0+(end_time_sec-start_time_sec)));
    //printf("Number of bins are %d. \n", hBSI->numberOfBins);
    //printHistogramBinStringInfo((struct histogramBinStringInfo *)hBSI);
    printLargeTextHistogramBinStringInfo(lTHBSI);

    //Freeing heap allocations
    free(lTHBSI->numberOfCharatersInBins);
    free(lTHBSI);
    for(int i = 0; i < uCI->numberOfThreads; i++)
    {
        free(tAIarr[i]);
        free(tRIarr[i]->numberOfCharatersInBins);
        free(tRIarr[i]);
    }
    free(tAIarr);
    free(tRIarr);
    free(uCI);
    free(lTSI);
    free(hBSI->numberOfCharatersInBins);
    free(hBSI);
    free(histogramBinString);
    free(largeTextString);

    printf("\n");
    return EXIT_SUCCESS;
}