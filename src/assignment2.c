#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 10
#define PAGE_SIZE 256
#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define PHYSICAL_SIZE 128
#define OFFSET_BITS 8

char buff[BUFFER_SIZE];
int search_TLB(int pageNumber);
void TLB_add(int i, int pageNumber, int frameNumber);
void TLB_Update(int index, int pageNumber, int frameNumber);
int hit;
int numHits;
int physicalAddress;
int pageNum;
int pageOff;

typedef struct TLBentry{
	int pageN;
	int frameN;
}TLBentry;

struct TLBentry TLB[TLB_SIZE];

int main(){
	int page;
	int b;
	int totaladdr;
	int i;
	int numFaults;
	int pageTable[PAGE_TABLE_SIZE];
	int offset;
	int tlbremove;
	int frameNum;

	tlbremove = 0;
	hit = 0;
	physicalAddress = 0;
	numHits = 0;
	totaladdr = 0;
	frameNum = 0;

	//initialize TLB
	for (i = 0; i < TLB_SIZE; i++){
		TLB[i].pageN = -1;
		TLB[i].frameN = -1;
	}
	//initialize page table
	for (i = 0; i < PAGE_TABLE_SIZE; i++){
		pageTable[i] = -1;
	}
	FILE *fptr = fopen("addresses.txt","r");
	while (fgets(buff,BUFFER_SIZE,fptr)!=NULL){
		for (i = 0; i <TLB_SIZE; i++){
			printf("%d, ", TLB[i].pageN);
		}
		hit = 0;
		b = atoi(buff);
		pageNum = b>>OFFSET_BITS;
		pageOff = b&255;
		totaladdr++;
		printf("page num: %d\n",pageNum);
		hit = search_TLB(pageNum);

		if (hit == 0){
			if (pageTable[pageNum] == -1){
				if (tlbremove >= TLB_SIZE){
					tlbremove = 0;
				}
				pageTable[pageNum] = frameNum;
				TLB_Update(tlbremove,pageNum,frameNum);
				frameNum++;
				tlbremove++;
				physicalAddress = (frameNum<<OFFSET_BITS) | pageOff;
			}
			else{
				physicalAddress = (pageTable[pageNum]<<OFFSET_BITS) | pageOff;
			}

		}
		printf("address: %s  physical address: %d\n",buff, physicalAddress);
	}
	printf("total addresses: %d \n", totaladdr);
	printf("hits: %d \n",numHits);
	fclose(fptr);
	return 0;
}
void TLB_add(int i, int pageNumber, int frameNumber){
	TLB[i].pageN = pageNumber;
	TLB[i].frameN = frameNumber;
}
int search_TLB(int pageNumber){
	int i;

	for (i = 0; i < TLB_SIZE; i++){
		if (TLB[i].pageN == pageNumber){
			numHits++;
			physicalAddress = (TLB[i].frameN<<OFFSET_BITS) | pageOff;
			return 1;
		}
	}
	return 0;
}
void TLB_Update(int i, int pageNumber, int frameNumber){
	TLB_add(i, pageNumber, frameNumber);
}