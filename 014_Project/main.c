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

int findCharFrequencyInString(const char character, const char * str)
{
    int frequency = 0;
    for(int i = 0; *(str+i)!='\0'; i++) if(*(str+i)==character) frequency++;
    return frequency;
}

struct histogramBinStringInfo * findHistogramBinStringInfo(char* str)
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
    printf("Your large text size is %d \n", strlen(largeTextString));
    free(buffer);
    //Reading histograms bin file in buffer then pasting back to histogramBinString
    buffer = malloc(BUFFER_SIZE);
    char * histogramBinString = malloc(PAGE_SIZE);
    while (fgets(buffer, BUFFER_SIZE, histogramBinsFile) != NULL) {
        strcat(histogramBinString, buffer);
    }
    printf("Your histogram bin string size is %d \n", strlen(histogramBinString));
    free(buffer);
    //
    printf("\n");
    struct histogramBinStringInfo * hBSI = findHistogramBinStringInfo(histogramBinString);
    printHistogramBinStringInfo((struct histogramBinStringInfo *)hBSI);
    //printf("Large text file has %c %d times. \n",'\n',findCharFrequencyInString('\n',largeTextString));
    printf("Done! \n");
    return EXIT_SUCCESS;
}