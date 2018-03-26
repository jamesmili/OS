#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#define B_SIZE 300
#define BYTES 4
#define R_SIZE 20

int requests[R_SIZE];
int sortedrequests[R_SIZE];
signed char *mmapfptr;
int header;
char direction[12];

int FIFO[R_SIZE];
int SSTF[R_SIZE];
int SCAN[R_SIZE];
int C_SCAN[R_SIZE];
int LOOK[R_SIZE];
int C_LOOK[R_SIZE];

void _SORT(void);
int _FIFO(void);
int _SSTF(void);
int _SCAN(void);
int _C_SCAN(void);
int _LOOK(void);
int _C_LOOK(void);

int main(int argc, char *argv[]){
  int i;
  int FIFO_HM = 0;
  int SSTF_HM = 0;
  int SCAN_HM = 0;
  int CSCAN_HM = 0;
  int LOOK_HM = 0;
  int CLOOK_HM = 0;

  header = atoi(argv[1]);
  strcpy(direction,argv[2]);
  printf("%d, %s\n", header, direction);

  int mmapfile_fd = open("request.bin", O_RDONLY);
  mmapfptr = mmap(0, B_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
  for (i = 0; i < B_SIZE; i++){
    memcpy(requests+i, mmapfptr+i*BYTES, BYTES);
  }
  close (mmapfile_fd);
  munmap(mmapfptr, B_SIZE);

  for (i = 0; i < R_SIZE; i++){
    sortedrequests[i] = requests[i];
  }

  FIFO_HM = _FIFO();
  printf("FIFO\n");
  for (i = 0; i < R_SIZE; i++){
    printf("%d\n", FIFO[i]);
  }
  printf("head_movements: %d\n", FIFO_HM);

  _SORT();
/*
  SSTF_HM = _SSTF();
  printf("SSTF\n");
  for (i = 0; i < R_SIZE; i++){
    printf("%d\n", SSTF[i]);
  }
  printf("head_movements: %d\n", SSTF_HM);
*/
  SCAN_HM = _SCAN();
  printf("SCAN\n");
  for (i = 0; i < R_SIZE; i++){
    printf("%d\n", SCAN[i]);
  }
  printf("head_movements: %d\n", SCAN_HM);


  CSCAN_HM = _C_SCAN();
  printf("C SCAN\n");
  for (i = 0; i < R_SIZE; i++){
    printf("%d\n", C_SCAN[i]);
  }
  printf("head_movements: %d\n", CSCAN_HM);


  LOOK_HM = _LOOK();
  printf("LOOK\n");
  for (i = 0; i < R_SIZE; i++){
    printf("%d\n", LOOK[i]);
  }
  printf("head_movements: %d\n", LOOK_HM);


  CLOOK_HM = _C_LOOK();
  printf("C LOOK\n");
  for (i = 0; i < R_SIZE; i++){
    printf("%d\n", C_LOOK[i]);
  }
  printf("head_movements: %d\n", CLOOK_HM);


  return 0;
}

void _SORT(){
  int i;
  int j;
  int min;
  int swap;
  int temp;
  for (i = 0; i<R_SIZE; i++){
    min = sortedrequests[i];
    for(j = i; j<R_SIZE; j++){
      if (sortedrequests[j] < min){
        min = sortedrequests[j];
        swap = j;
      }
    }
    if (min != sortedrequests[i]){
      temp = sortedrequests[i];
      sortedrequests[i] = min;
      sortedrequests[swap] = temp;
    }
  }
}

int _FIFO(){
  int h = header;
  int head_movements = 0;
  int i;

  for (i = 0; i < R_SIZE; i++){
    FIFO[i] = requests[i];
    head_movements += abs(h - FIFO[i]);
    h = FIFO[i];
  }
  return head_movements;
}
/*
int _SSTF(){
  int i;
  int start;
  int temp;
  int index;
  int h = header;
  int head_movements = 0;
  int count = 0;
  temp = 0;
  for(i = 0; i<R_SIZE; i++){
    if (header < sortedrequests[i+1]){
      index = i;
      break;
    }
  }
  while(count!=19){
    if (abs(h - sortedrequests[index]) > abs(h-sortedrequests[index+1])){
      SSTF[count] = sortedrequests[index+1];
      h = sortedrequests[index+1];
      index++;
    }else if (abs(h - sortedrequests[index]) < abs(h-sortedrequests[index+1])){
      SSTF[count] = sortedrequests[index];
    }else{
      if (strcmp(direction,"LEFT") == 0){
        SSTF[count] = sortedrequests[index];
      }else{
        SSTF[count] = sortedrequests[index+1];
      }
    }
    count++;
  }
  return head_movements;
}
*/
int _SCAN(){
  int i;
  int start;
  int temp;
  int index;
  int h = header;
  int head_movements = 0;

  temp = 0;
  if (strcmp(direction,"LEFT") == 0){
    for(i = 0; i<R_SIZE; i++){
      if (header < sortedrequests[i+1]){
        index = i;
        break;
      }
    }
    for (i = index; i>=0; i--){
      SCAN[temp] = sortedrequests[i];
      temp++;
    }
    for (i = index+1; i<R_SIZE; i++){
      SCAN[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header-0) + abs(0-sortedrequests[19]);
  }else{
    for(i = 0; i<R_SIZE; i++){
      if (header < sortedrequests[i+1]){
        index = i;
        break;
      }
    }
    for (i = index+1; i<R_SIZE; i++){
      SCAN[temp] = sortedrequests[i];
      temp++;
    }
    for (i = index; i>=0; i--){
      SCAN[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header-299) + abs(sortedrequests[19]-sortedrequests[0]);
  }
  return head_movements;
}

int _C_SCAN(){
  int i;
  int start;
  int temp;
  int index;
  int h = header;
  int head_movements = 0;
  temp = 0;
  for(i = 0; i<R_SIZE; i++){
    if (header < sortedrequests[i+1]){
      index = i;
      break;
    }
  }
  if (strcmp(direction,"LEFT") == 0){
    for (i = index; i>=0; i--){
      C_SCAN[temp] = sortedrequests[i];
      temp++;
    }
    for (i = R_SIZE-1; i>=index+1; i--){
      C_SCAN[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header-0) + abs(0-299) + abs(299-sortedrequests[index+1]);
  }else{
    for (i = index+1; i<R_SIZE; i++){
      C_SCAN[temp] = sortedrequests[i];
      temp++;
    }
    for (i = 0; i<=index; i++){
      C_SCAN[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header-299) + abs(299-0) + abs(0-sortedrequests[index]);
  }
  return head_movements;
}

int _LOOK(){
  int i;
  int start;
  int temp;
  int index;
  int h = header;
  int head_movements = 0;
  temp = 0;
  if (strcmp(direction,"LEFT") == 0){
    for(i = 0; i<R_SIZE; i++){
      if (header < sortedrequests[i+1]){
        index = i;
        break;
      }
    }
    for (i = index; i>=0; i--){
      LOOK[temp] = sortedrequests[i];
      temp++;
    }
    for (i = index+1; i<R_SIZE; i++){
      LOOK[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header - sortedrequests[0]) +  abs(sortedrequests[0]-sortedrequests[19]);
  }else{
    for(i = 0; i<R_SIZE; i++){
      if (header < sortedrequests[i+1]){
        index = i;
        break;
      }
    }
    for (i = index+1; i<R_SIZE; i++){
      LOOK[temp] = sortedrequests[i];
      temp++;
    }
    for (i = index; i>=0; i--){
      LOOK[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header - sortedrequests[19]) +  abs(sortedrequests[19]-sortedrequests[0]);
  }
  return head_movements;
}

int _C_LOOK(){
  int i;
  int start;
  int temp;
  int index;
  int h = header;
  int head_movements = 0;
  temp = 0;
  for(i = 0; i<R_SIZE; i++){
    if (header < sortedrequests[i+1]){
      index = i;
      break;
    }
  }
  if (strcmp(direction,"LEFT") == 0){
    for (i = index; i>=0; i--){
      C_LOOK[temp] = sortedrequests[i];
      temp++;
    }
    for (i = R_SIZE-1; i>=index+1; i--){
      C_LOOK[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header - sortedrequests[0]) +  abs(sortedrequests[0]-sortedrequests[19]) + abs(sortedrequests[19]-sortedrequests[index+1]);
  }else{
    for (i = index+1; i<R_SIZE; i++){
      C_LOOK[temp] = sortedrequests[i];
      temp++;
    }
    for (i = 0; i<=index; i++){
      C_LOOK[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header - sortedrequests[19]) +  abs(sortedrequests[19]-sortedrequests[0]) + abs(sortedrequests[0]-sortedrequests[index]);
  }
  return head_movements;
}
