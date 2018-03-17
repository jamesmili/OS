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
#define MEM_SIZE 128*PAGE_SIZE

int search_TLB(int pageNumber);
void TLB_add(int i, int pageNumber, int frameNumber);
void TLB_Update(int pageNumber, int frameNumber);

signed char *mmapfptr;
char buff[BUFFER_SIZE];
int hit;
int numHits;
int physicalAddress;
int pageNum;
int pageOff;
int tlbremove;

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
	int frameNum;
	int value;
	int freeFrame;

	tlbremove = 0;
	hit = 0;
	physicalAddress = 0;
	numHits = 0;
	totaladdr = 0;
	frameNum = 0;
	numFaults = 0;
	value = 0;
	freeFrame = 0;

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
	FILE *out = fopen("output.txt","w");
	int mmapfile_fd = open("BACKING_STORE.bin", O_RDONLY);
	mmapfptr = mmap(0, PAGE_SIZE*PAGE_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
	while (fgets(buff,BUFFER_SIZE,fptr)!=NULL){
		hit = 0;
		logicalAddress = atoi(buff);
		pageNum = logicalAddress>>OFFSET_BITS;
		pageOff = logicalAddress&255;
		totaladdr++;
		//Check TLB
		hit = search_TLB(pageNum);
		printf("frameNum: %d, frameNum*PAGE_SIZE: %d\n", frameNum, frameNum*PAGE_SIZE);

		//TLB Miss
		if (hit == 0){
			//Check if frame is in page table
			//page fault
			if (pageTable[pageNum] == -1){
				printf("page fault ");
				//if no frames are available, start at index 0 to achieve FIFO
				if (frameNum*PAGE_SIZE >= MEM_SIZE){
					printf("reset frame num\n");
					frameNum = 0;
				}
				memcpy(physicalMemory+frameNum*PAGE_SIZE, mmapfptr+pageNum*PAGE_SIZE, PAGE_SIZE);
				value = physicalMemory[frameNum+pageOff];
				pageTable[pageNum] = frameNum;
				TLB_Update(pageNum,frameNum);
				physicalAddress = frameNum*PAGE_SIZE + pageOff;
				frameNum++;
				numFaults++;
			}else{
				//get frame number from pagetable
				printf("page miss ");
				TLB_Update(pageNum, pageTable[pageNum]);
				physicalAddress = pageTable[pageNum]*PAGE_SIZE + pageOff;
			}
		}
		printf("%d PN: %d, Offset: %d, FN: %d, PA: %d VAL: %d\n", totaladdr,pageNum, pageOff, pageTable[pageNum],physicalAddress, physicalMemory[pageNum+offset]);
		fprintf(out, "Virtual Address: %d ", logicalAddress);
		fprintf(out, "Physical Address = %d ", physicalAddress);
		fprintf(out, "Value= %d\n", physicalMemory[pageNum+pageOff]);
	}
	fprintf(out, "Total Addresses = %d \n", totaladdr);
	fprintf(out, "Page_faults = %d\n", numFaults);
	fprintf(out, "TLB hits = %d \n",numHits);
	close (mmapfile_fd);
	fclose(out);
	fclose(fptr);
	munmap(mmapfptr, MEM_SIZE);
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
			printf("page hit ");
			physicalAddress = TLB[i].frameN*PAGE_SIZE + pageOff;
			return 1;
		}
	}
	return 0;
}
void TLB_Update(int pageNumber, int frameNumber){
	// help with the circular array
	if (tlbremove >= TLB_SIZE){
		tlbremove = 0;
	}
	TLB_add(tlbremove, pageNumber, frameNumber);
	tlbremove++;

}
