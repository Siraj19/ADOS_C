#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

//Warning that the page size should be larger than your largetextfile
#define PAGE_SIZE 4096
#define BUFFER_SIZE 2

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
    for(int bin = 0; bin<lTHBSI->numberOfBins-1; bin++)
    {
        printf("Number of characters in %d bin is %d. \n", bin, lTHBSI->numberOfCharatersInBins[bin]);
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
    printf("Hello, I am thread %d \n", tAI->threadID);
    //Return with 0 count instead of doing segmentation fault due to limitation of large text string
    if ((tAI->threadID)*(tAI->uCI->blockSize)>(tAI->lTSI->size))
    {
        return (void *) tRI;
    }

    for(int i = (tAI->threadID)*(tAI->uCI->blockSize); i<(tAI->threadID+1)*(tAI->uCI->blockSize); i++)
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

int main(int argc, char *argv[]) {
    //Checking if number of input arguments are complete
    if(argc != 5){
        fprintf(stderr, "Number of arguments are %d instead of 4.\n", argc-1);
        exit(EXIT_FAILURE);
    }
    //Assigning pointers to their respective addresses
    const char * pathLargeTextFile = argv[1];
    const char * pathHistogramBinsFile = argv[2];
    //Converting asci string character to their respective equivalent integer
    const int numberOfThreads = atoi(argv[3]);
    const int blockSize = atoi(argv[4]);
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
    //Reading large text file in buffer then pasting back to largeTextString
    char * buffer = malloc(BUFFER_SIZE);
    char * largeTextString = malloc(PAGE_SIZE);
    while (fgets(buffer, BUFFER_SIZE, largeTextFile) != NULL) {
        strcat(largeTextString, buffer);
    }
    printf("Your large text size is %d \n", (int)strlen(largeTextString));
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
    //

    for(int i = 0; i < uCI->numberOfThreads; i++)
    {
        tAIarr[i] = packthreadArgInfo(hBSI, lTSI, uCI, i);
    }
    //Creating threads
    pthread_t p[uCI->numberOfThreads];
    
    for(int i = 0; i < uCI->numberOfThreads; i++)
    {
        pthread_create(&p[i], NULL, (void *)sectionedPartioningWorkerFunction, tAIarr[i]);
    }

    for(int i = 0; i < uCI->numberOfThreads; i++)
    {
        pthread_join(p[i], (void **) &tRIarr[i]);
        printf("Thread %d returned back to the main. \n", tRIarr[i]->threadID);
        for(int b = 0; b < lTHBSI->numberOfBins-1; b++)
        {
            lTHBSI->numberOfCharatersInBins[b] += tRIarr[i]->numberOfCharatersInBins[b];
            lTHBSI->numberOfCharatersInBins[lTHBSI->numberOfBins-1] -= tRIarr[i]->numberOfCharatersInBins[b];
        }
    }
    printf("Number of bins are %d. \n", hBSI->numberOfBins);
    //printHistogramBinStringInfo((struct histogramBinStringInfo *)hBSI);
    printLargeTextHistogramBinStringInfo(lTHBSI);
    printf("Done! \n");
    return EXIT_SUCCESS;
}