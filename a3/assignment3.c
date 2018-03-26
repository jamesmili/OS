#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#define B_SIZE 300
#define BYTES 4
#define R_SIZE 20

int requests[R_SIZE];
signed char *mmapfptr;
int header;
char direction[12];

void _SORT(void);
int * _FIFO(void);
int * _SSTF(void);
int * _SCAN(void);
int * _C_SCAN(void);
int * _LOOK(void);
int * _C_LOOK(void);

int main(int argc, char *argv[]){

  int *FIFO;
  int *SSTF;
  int *SCAN;
  int *C_SCAN;
  int *LOOK;
  int *C_LOOK;


  header = atoi(argv[1]);
  strcpy(direction,argv[2]);
  printf("%d, %s\n", header, direction);

  int h = header;
  int head_movements = 0;

  int mmapfile_fd = open("request.bin", O_RDONLY);
  mmapfptr = mmap(0, B_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
  int i;
  for (i = 0; i < B_SIZE; i++){
    memcpy(requests+i, mmapfptr+i*BYTES, BYTES);
  }
  close (mmapfile_fd);
  munmap(mmapfptr, B_SIZE);
  FIFO = _FIFO();
  printf("FIFO\n");
  for (i = 0; i < R_SIZE; i++){
    printf("%d\n", *(FIFO+i));
    head_movements += abs(h - *(FIFO+i));
    h = *(FIFO+i);
  }
  printf("head_movements: %d\n", head_movements);
  _SORT();
  printf("SORT\n");
  for (i = 0; i < R_SIZE; i++){
    printf("%d\n", requests[i]);
  }
  /*
  SCAN = _SCAN();
  head_movements = 0;
  h = header;
  for (i = 0; i < R_SIZE; i++){
    printf("%d\n", *(SCAN+i));
    head_movements += abs(h - *(SCAN+i));
    h = *(SCAN+i);
  }
  */

  return 0;
}

void _SORT(){
  int i;
  int j;
  int min;
  int swap;
  int temp;
  for (i = 0; i<R_SIZE; i++){
    min = requests[i];
    for(j = i; j<R_SIZE; j++){
      if (requests[j] < min){
        min = requests[j];
        swap = j;
      }
    }
    if (min != requests[i]){
      temp = requests[i];
      requests[i] = min;
      requests[swap] = temp;
    }
  }
}


int * _FIFO(){
  return requests;
}

int * _SCAN(){
  int i;
  int scn[R_SIZE];
  int start;
  int temp;
  int count = 0;
  if (strcmp(direction,"LEFT") == 0){
    for (i = 0; i < R_SIZE; i++){
      if (header > requests[i]){
        start = i;
      }else{
        break;
      }
    }
    for (i = start; i>=requests[0]; i--){
        scn[count] = requests[i];
        count++;
    }
    for (i = start; i<=requests[19]; i++){
      scn[count] = requests[i];
      count++;
    }
  }
  return scn;
}
