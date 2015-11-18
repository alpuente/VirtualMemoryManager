#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int *pageNums; 
int *offsets;
int *pageTable;
int *lAddresses;
char **frames;

int *toB(int n) {
  int c, k, i;
  int *bNum = malloc(8*sizeof(int));
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
  
  /*printf("\n");
  for (int j = 0; j < 8; j++) {
    printf("%d", bNum[j]);
  }
  printf("\n");*/

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
        lAddresses[index] = atoi(line);
        index++;
    }
    return 0;
}

char *getPage(int pageNum, int offset, char *bytes) {
    FILE *fp = fopen("BACKING_STORE.bin", "rb");
    fseek(fp, (pageNum*256), SEEK_SET);
    fread(bytes, 1, 256, fp);
    fclose(fp);
    return bytes;
}


int logicalToPhysical(int index) {
    double exp = 0; // keep track of the power we want to raise 2 to
    int *frameB = malloc(sizeof(int)*8);
    int *offsetB = malloc(sizeof(int)*8);
    int pageNum = pageNums[index];
    frameB = toB(pageTable[pageNum]);
    offsetB = toB(offsets[index]);

    int physicalAddress = 0;
    for (int i = 7; i >= 0; i--) {
        if (offsetB[i] == 1) {
            //printf("%f\n", pow(2,exp));
            physicalAddress += (int) pow(2, exp);
        }
        exp++;
    }
    
    for (int j = 7; j >= 0; j--) {
        if (frameB[j] == 1) {
            physicalAddress += (int) pow(2, exp);
            //printf("%f\n", pow(2,exp));
        }
        exp++;
    }
    //printf("physical address %d\n", physicalAddress);
    return physicalAddress;
}


int main (int argc, char **argv) {
    int physicalAddress, signedByte;
    pageTable = malloc(256 * sizeof(int));
    pageNums = malloc(1000 * sizeof(int));
    offsets = malloc(1000 * sizeof(int));
    lAddresses = malloc(256 * sizeof(int));
    frames = malloc(256 * sizeof(char*));
    readFile("input.txt");
    for (int k = 0; k < 256; k++) {
        frames[k] = malloc( sizeof(char) * 256);
    }
    
    for (int j = 0; j < 256; j++) {
        pageTable[j] = -1;
    }
    
    int frameNum = 0;
    for (int i = 0; i < 1000; i++) {
        int pageNum = pageNums[i];
        int offset = offsets[i];
        
        if (pageTable[pageNum] == -1) {
            pageTable[pageNum] = frameNum;
            physicalAddress = logicalToPhysical(i);
            char *f = malloc(256 * sizeof(char));
            f = getPage(pageNum, offset, f);
            frames[frameNum] = f;
            signedByte = (int) frames[frameNum][offset];
            frameNum++;
        } else {
            physicalAddress = logicalToPhysical(i);
            int fNum  = pageTable[pageNum];
            signedByte = (int) frames[fNum][offset];
        }

        printf("Virtual address: %d Physical address: %d Value: %d\n", lAddresses[i], physicalAddress, signedByte );
    }

    return 0;
}
