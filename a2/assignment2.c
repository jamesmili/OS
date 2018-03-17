#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 10
#define PAGE_SIZE 256
#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define OFFSET_BITS 8
#define MEM_SIZE 256*256

signed char *mmapfptr;
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
	int logicalAddress;
	int totaladdr;
	int i;
	int numFaults;
	int pageTable[PAGE_TABLE_SIZE];
	char physicalMemory[MEM_SIZE];
	int offset;
	int tlbremove;
	int frameNum;
	int value;

	tlbremove = 0;
	hit = 0;
	physicalAddress = 0;
	numHits = 0;
	totaladdr = 0;
	frameNum = 0;
	numFaults = 0;
	value = 0;

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
	int mmapfile_fd = open("BACKING_STORE.bin", O_RDONLY);
	mmapfptr = mmap(0, MEM_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
	while (fgets(buff,BUFFER_SIZE,fptr)!=NULL){
		hit = 0;
		logicalAddress = atoi(buff);
		pageNum = logicalAddress>>OFFSET_BITS;
		pageOff = logicalAddress&255;
		totaladdr++;

		//Check TLB
		hit = search_TLB(pageNum);

		//TLB Miss
		if (hit == 0){
			//Check if frame is in page table
			if (pageTable[pageNum] == -1){
				//page fault
				if (tlbremove >= TLB_SIZE){
					tlbremove = 0;
				}
				memcpy(physicalMemory+frameNum+pageOff, mmapfptr+pageNum*PAGE_SIZE, PAGE_SIZE);
				value = physicalMemory[frameNum+pageOff];
				pageTable[pageNum] = frameNum;
				TLB_Update(tlbremove,pageNum,frameNum);
				frameNum++;
				tlbremove++;
				numFaults++;
			}
			physicalAddress = pageTable[pageNum]*PAGE_SIZE + pageOff;
		}
		printf("Virtual Address: %d  Physical Address= %d Value=%d\n", logicalAddress, physicalAddress, physicalMemory[pageNum+pageOff]);
	}
	printf("Total Addresses = %d \n", totaladdr);
	printf("Page_faults = %d\n", numFaults);
	printf("TLB hits = %d \n",numHits);
	close (mmapfile_fd);
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
			physicalAddress = TLB[i].frameN*PAGE_SIZE + pageOff;
			return 1;
		}
	}
	return 0;
}
void TLB_Update(int i, int pageNumber, int frameNumber){
	TLB_add(i, pageNumber, frameNumber);
}
