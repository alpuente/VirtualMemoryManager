#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int *pageNums; 
int *offsets;
int *pageTable;

int *toB(int n) {
  int c, k, i;
  int *bNum = malloc(8*sizeof(int));
  n = 20;
  i = 0;
  int index = 0;
  for (c = 31; c >= 0; c--)
  {
    k = n >> c;

    if (k & 1) {
      if ( 31-i < 8) {
        bNum[index] = 1;
        index++;
      }
    }
    else {
      if (31-i < 8) {
        bNum[index] = 0;
        index++;
        }
    }
    i++;
  }
  
  printf("\n");
  for (int j = 0; j < 8; j++) {
    printf("%d", bNum[j]);
  }
  printf("\n");

    return bNum;
}


/**
* reads a file containing 32 bit integers and returns an array of
* binary numbers represented as integer arrays
**/
int readFile(char *file) {
    FILE *fp;
    fp = fopen(file,"r");
    char *line = malloc(sizeof(int)*12);
    ssize_t read;
    size_t len = 0;
    int index = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        int offset = (atoi(line) >> (8*0)) & 0xff;
        int pageN = (atoi(line) >> (8*1)) & 0xff;
        offsets[index] = offset;
        pageNums[index] = pageN;
        printf("\n");
        index++;
    }
    return 0;
}


int logicalToPhysical(int index, int *pageNums, int *offsets) {
    int frameNum = index;
    int offset = offsets[index];
    int *frameB = malloc(sizeof(int)*8);
    int *offsetB = malloc(sizeof(int)*8);
    frameB = toB(frameNum);
    offsetB = toB(offset);
    for (int i = 0; i < 8; i++) {
        printf("%d", frameB[i]);
    }
    printf(" ");
    for (int j = 0; j < 8; j++) {
        printf("%d", offsetB[j]);
    }   
    printf("\n");
    return 0;
}


int main (int argc, char **argv) {
    pageTable = malloc(256*sizeof(int));
    pageNums = malloc(1000 * sizeof(int));
    offsets = malloc(1000 * sizeof(int));
    readFile("input.txt");
    toB(20);
    /*
    for (int i = 0; i < 7; i++) {
        printf("%d %d\n", pageNums[i], offsets[i]);
    }*/
    printf("first and second\n");
    printf(" %d %d\n", pageNums[0], offsets[0]);
    logicalToPhysical(0, pageNums, offsets);
    printf(" %d  %d\n", pageNums[1], offsets[1]);
    logicalToPhysical(1, pageNums, offsets);
    
    return 0;
}
