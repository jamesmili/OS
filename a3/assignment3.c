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

int FCFS[R_SIZE];
int SSTF[R_SIZE];
int SCAN[R_SIZE];
int C_SCAN[R_SIZE];
int LOOK[R_SIZE];
int C_LOOK[R_SIZE];

void _SORT(void);
int _FCFS(void);
int _SSTF(void);
int _SCAN(void);
int _C_SCAN(void);
int _LOOK(void);
int _C_LOOK(void);

int main(int argc, char *argv[]){
  int i;
  int FCFS_HM = 0;
  int SSTF_HM = 0;
  int SCAN_HM = 0;
  int CSCAN_HM = 0;
  int LOOK_HM = 0;
  int CLOOK_HM = 0;
  char filename[100];

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
  if (strcmp(direction,"LEFT") == 0){
    strcpy(filename,"outputLEFT.txt");
  }else{
    strcpy(filename,"outputRIGHT.txt");
  }
  printf("filename: %s", filename);
  FILE *out = fopen(filename,"w");
  fprintf(out, "Total requests = %d \n", R_SIZE);
  fprintf(out, "Initial Head Position: %d \n", header);
  fprintf(out, "Direction of Head: %s \n", direction);

  FCFS_HM = _FCFS();
  fprintf(out,"\nFCFS DISK SCHEDULING ALGORITHM:\n\n");

  for (i = 0; i < R_SIZE; i++){
    fprintf(out,"%d, ", FCFS[i]);
  }
  fprintf(out,"\n\nFCFS - Total head movements = %d\n\n", FCFS_HM);

  _SORT();

  SSTF_HM = _SSTF();
  fprintf(out, "SSTF DISK SCHEDULING ALGORITHM\n\n");

  for (i = 0; i < R_SIZE; i++){
    fprintf(out,"%d, ", SSTF[i]);
  }
  fprintf(out,"\n\nSSTF - Total head movements = %d\n\n", SSTF_HM);

  SCAN_HM = _SCAN();
  fprintf(out, "SCAN DISK SCHEDULING ALGORITHM\n\n");

  for (i = 0; i < R_SIZE; i++){
    fprintf(out,"%d, ", SCAN[i]);
  }
  fprintf(out,"\n\nSCAN - Total head movements = %d\n\n", SCAN_HM);


  CSCAN_HM = _C_SCAN();
  fprintf(out, "C-SCAN DISK SCHEDULING ALGORITHM\n\n");

  for (i = 0; i < R_SIZE; i++){
    fprintf(out,"%d, ", C_SCAN[i]);
  }
  fprintf(out,"\n\nC-SCAN - Total head movements = %d\n\n", CSCAN_HM);

  LOOK_HM = _LOOK();
  fprintf(out, "LOOK DISK SCHEDULING ALGORITHM\n\n");

  for (i = 0; i < R_SIZE; i++){
    fprintf(out,"%d, ", LOOK[i]);
  }
  fprintf(out,"\n\nLOOK - Total head movements = %d\n\n", LOOK_HM);

  CLOOK_HM = _C_LOOK();
  fprintf(out, "C-LOOK DISK SCHEDULING ALGORITHM\n\n");

  for (i = 0; i < R_SIZE; i++){
    fprintf(out,"%d, ", C_LOOK[i]);
  }
  fprintf(out,"\n\nC-LOOK - Total head movements = %d\n", CLOOK_HM);
  fclose(out);

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

int _FCFS(){
  int h = header;
  int head_movements = 0;
  int i;

  for (i = 0; i < R_SIZE; i++){
    FCFS[i] = requests[i];
    head_movements += abs(h - FCFS[i]);
    h = FCFS[i];
  }
  return head_movements;
}

int _SSTF(){
  int i;
  int start;
  int temp;
  int index;
  int h = header;
  int head_movements = 0;
  int count = 0;
  int left;
  int right;
  int l = 0;
  int r = 0;
  int flag = 0;
  int flag2 = 0;
  temp = 0;
  for(i = 0; i<R_SIZE; i++){
    if (header < sortedrequests[i+1]){
      index = i;
      break;
    }
  }
  left = sortedrequests[index];
  right = sortedrequests[index+1];
  while(count<R_SIZE){
    if (flag == 1){
      for (i = index+1; i<R_SIZE; i++){
        SSTF[count] = sortedrequests[i];
        count++;
      }
    }
    else if (flag2 == 1){
      for (i = index; i>=0; i--){
        SSTF[count] = sortedrequests[i];
        count++;
      }
    }
    else if (left == sortedrequests[0]){
      SSTF[count] = left;
      flag = 1;
    }
    else if (right == sortedrequests[19]){
      SSTF[count] = right;
      flag2 = 1;
    }
    else if (abs(h - left) > abs(h-right)){
      SSTF[count] = right;
      h = right;
      l-=2;
      r++;
      left = sortedrequests[index+l];
      right = sortedrequests[index+r];
    }else if (abs(h - left) < abs(h-right)){
      SSTF[count] = left;
      l--;
      r+=2;
      left = sortedrequests[index+l];
      right = sortedrequests[index+r];
    }else{
      if (strcmp(direction,"LEFT") == 0){
        SSTF[count] = left;
        l--;
        r+=2;
        left = sortedrequests[index+l];
        right = sortedrequests[index+r];
      }else{
        SSTF[count] = right;
        l-=2;
        r++;
        left = sortedrequests[index+l];
        right = sortedrequests[index+r];
      }
    }
    count++;
  }
  for (i = 0; i <R_SIZE; i++){
    head_movements += abs(h - SSTF[i]);
    h = SSTF[i];
  }
  return head_movements;
}

int _SCAN(){
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
      SCAN[temp] = sortedrequests[i];
      temp++;
    }
    for (i = index+1; i<R_SIZE; i++){
      SCAN[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header-0) + abs(0-sortedrequests[19]);
  }else{
    for (i = index; i<R_SIZE; i++){
      SCAN[temp] = sortedrequests[i];
      temp++;
    }
    for (i = index-1; i>=0; i--){
      SCAN[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header-299) + abs(299-sortedrequests[0]);
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
    for (i = index; i<R_SIZE; i++){
      C_SCAN[temp] = sortedrequests[i];
      temp++;
    }
    for (i = 0; i<=index-1; i++){
      C_SCAN[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header-299) + abs(299-0) + abs(0-sortedrequests[index-1]);
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
  for(i = 0; i<R_SIZE; i++){
    if (header < sortedrequests[i+1]){
      index = i;
      break;
    }
  }
  if (strcmp(direction,"LEFT") == 0){
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
    for (i = index; i<R_SIZE; i++){
      LOOK[temp] = sortedrequests[i];
      temp++;
    }
    for (i = index-1; i>=0; i--){
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
    for (i = index; i<R_SIZE; i++){
      C_LOOK[temp] = sortedrequests[i];
      temp++;
    }
    for (i = 0; i<=index; i++){
      C_LOOK[temp] = sortedrequests[i];
      temp++;
    }
    head_movements = abs(header - sortedrequests[19]) +  abs(sortedrequests[19]-sortedrequests[0]) + abs(sortedrequests[0]-sortedrequests[index-1]);
  }
  return head_movements;
}
