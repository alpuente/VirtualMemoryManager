#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int *pageNums; 
int *offsets;
int *pageTable;
int *lAddresses;
char **frames;
int **tlb;

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

/*
* returns frame number if page is in tlb and -1 if it isn't
*/
int checkTLB(int pageNum) { 
    for (int i = 0; i < 16; i++) {
        if (pageNum == tlb[i][0]) {
            return tlb[i][1]; // return frame number if found
        }
    }
    return -1;
}

/*
* updates the TLB at the relevant index, keeping the TLB in FIFO order
*/
void updateTLB(int pageNum, int frameNum, int index) {
    index = index % 16;
    tlb[index][0] = pageNum;
    tlb[index][1] = frameNum;
}

/*
* returns the physical address when supplied the page's index in the original file
*/
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
    int tlbIndex = 0;
    int pageTableHits = 0;
    int tlbHits = 0;
    int pageFaults = 0;
    int totalAddresses = 0;
    pageTable = malloc(256 * sizeof(int));
    pageNums = malloc(1000 * sizeof(int));
    offsets = malloc(1000 * sizeof(int));
    lAddresses = malloc(256 * sizeof(int));
    frames = malloc(256 * sizeof(char*));
    tlb = malloc(16 * sizeof(int*));

    for (int l = 0; l < 16; l++) {
        tlb[l] = malloc(2 * sizeof(int));
    }

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

        int tlbCheck = checkTLB(pageNum);
        if (tlbCheck != -1) { // check if page is in tlb first
            
            physicalAddress = logicalToPhysical(i);
            signedByte = (int) frames[tlbCheck][offset];
            tlbHits++;
        
        } else if (pageTable[pageNum] == -1) { // pageNumber hasn't been seen before
            
            pageTable[pageNum] = frameNum;
            physicalAddress = logicalToPhysical(i);
            char *f = malloc(256 * sizeof(char));
            f = getPage(pageNum, offset, f);
            frames[frameNum] = f;
            signedByte = (int) frames[frameNum][offset];
            updateTLB(pageNum, frameNum, tlbIndex);
            tlbIndex++;
            frameNum++;
            pageFaults++;

        } else { // get the page from the pageTable

            physicalAddress = logicalToPhysical(i);
            int fNum  = pageTable[pageNum];
            signedByte = (int) frames[fNum][offset];
            updateTLB(pageNum, fNum, tlbIndex);
            tlbIndex++;
            pageTableHits++;

        }

        printf("Virtual address: %d Physical address: %d Value: %d\n", lAddresses[i], physicalAddress, signedByte );
        totalAddresses++;
    }

    printf("Number of Translated Addresses = %d\n", totalAddresses);
    printf("Page Faults = %d\n", pageFaults);
    printf("Page Fault Rate = %.3f\n", (double) pageFaults / 1000 );
    printf("TLB Hits = %d\n", tlbHits);
    printf("TLB Hit Rate = %.3f\n", (double) tlbHits / 1000);

    return 0;
}
