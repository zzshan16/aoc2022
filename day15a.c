#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "simple.h"
int process_fun(char* map, size_t size){
  size_t* offset_arr = find_splits_str(map, size, "\n");
  char* charptr;
  if (!offset_arr){puts("something wrong"); exit(1);}
  int t = 0;
  while(charptr = load_next_split(map, offset_arr, size)){//use set_pos(1) to ignore first segment
    ++t;
    // printf("%ld\n", atol(charptr));
    //printf("line number %d; offset = %lu\n",t, offset_arr[t]);
    //printf("%s", charptr);
  }
  free_ebuf();
  free(offset_arr);
  return 0;
}
int row = 2000000;
typedef struct sen{
  int x;
  int y;
  int dist;
} sen;
void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  int num_sens = 0;
  sen sens[100];
  int *arr = malloc(10000000*sizeof(int));
  memset(arr, 0, 10000000*sizeof(int));
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    /* printf("%s\n", buf); */
    //printf("%ld\n", strlen(buf));
    int sensx;
    int sensy;
    int beax;
    int beay;
    int dist;
    int a = sscanf(buf, "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", &sensx, &sensy, &beax, &beay);
    if (a == 4){
      dist = abs(beax - sensx) + abs(beay-sensy);
      sens[num_sens].x = sensx;
      sens[num_sens].y = sensy;
      sens[num_sens].dist = dist;
      num_sens++;
    }
    if (beay == row){
      arr[beax+3000000] = 1;
    }
  }
  puts("");
  int count;
  for(int i = 0; i < num_sens; ++i){
    /* printf("%d %d %d\n", sens[i].x, sens[i].y, sens[i].dist); */
    int dy = abs(row - sens[i].y);
    int remain = sens[i].dist - dy;
    int x = sens[i].x;
    if (remain < 0) continue;
    arr[x+3000000] |= 2;
    while (remain){
      arr[x-remain--+3000000] |= 2;
    }
    remain = sens[i].dist - dy;
    while (remain){
      arr[x+remain--+3000000] |= 2;
    }
  }
  count = 0;
  for(int i = 0; i < 10000000; ++i){
    if (arr[i] & 1) continue;
    if (arr[i] & 2) count++;
  }
  printf("count = %d\n", count);

  /* for(int i = 0; i < vectp->size; i+= vectp->element_size){ */
  /*   printf("%d: %d \n", i, *(int*)(vectp->head+i)); */
  /* } */
  
  fclose(fp);
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  process_fun(input_map, filesize);
  process_fgets();
  munmap(input_map, filesize);
  return 0;
}
