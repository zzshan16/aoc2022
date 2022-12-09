extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
}
#include <unordered_set>
#include <cstdint>
int process_fun(char* map, int size){
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
void calculate_new_pos(int16_t x, int16_t y, int16_t* x0, int16_t* y0){
  if (x-*x0 > 1 && y-*y0 > 1){
    *x0 = x-1;
    *y0 = y-1;
    return;
  }
  if (x-*x0 > 1 && y-*y0 < -1){
    *x0 = x-1;
    *y0 = y+1;
    return;
  }
  if (x-*x0 < -1 && y-*y0 > 1){
    *x0 = x+1;
    *y0 = y-1;
    return;
  }
  if (x-*x0 < -1 && y-*y0 < -1){
    *x0 = x+1;
    *y0 = y+1;
    return;
  }
    
  if (x-*x0 > 1){
    *y0 = y;
    *x0 = x-1;
  }
  else if (x-*x0 < -1){
    *y0 = y;
    *x0 = x+1;
  }
  else if (y-*y0 < -1){
    *x0 = x;
    *y0 = y+1;
  }
  else if (y-*y0 > 1){
    *x0 = x;
    *y0 = y-1;
  }
}
void process_fgets(){
    FILE* fp = fopen("input", "rw");
  char buf[32768];
  char dir;
  int dist;
  int16_t x = 0;
  int16_t y = 0;
  int16_t x0 = 0;
  int16_t y0 = 0;
  int16_t arrx[9];
  int16_t arry[9];
  std::unordered_set<uint32_t> set;
  // set.insert(0);
  uint32_t temp;
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    if (strlen(buf) < 3){
      printf("ignoring line\n");
      continue;
    }
    dir = *buf;
    dist = atoi(buf+2);
    while(dist){
      switch(dir){
      case 'U':
	y++;
	break;
      case 'D':
	y--;
	break;
      case 'L':
	x--;
	break;
      case 'R':
	x++;
	break;
      }
      //  int16_t arry[9];
      calculate_new_pos(x, y, arrx, arry);
      for(int i = 1; i < 9; ++i){
	calculate_new_pos(arrx[i-1], arry[i-1], arrx+i, arry+i);
      }
      // printf(" head (%d, %d) tail (%d,%d)\n", x, y, x0, y0);
      temp = (uint16_t)arrx[8];
      temp |= (uint16_t)arry[8] << 16ul;
      set.insert(temp);
      // printf("inserting %X\n", temp);
      //printf("num = %lu\n", set.size());
      dist--;
    }
    for(int i = 0; i < 9; ++i){
      printf("%d %d %d\n", i, arrx[i], arry[i]);
    }
  }
  printf("num = %lu\n", set.size());
  fclose(fp);
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = (char*)mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  
  //  process_fun(input_map, filesize);
  process_fgets();
  munmap(input_map, filesize);
  return 0;
}
