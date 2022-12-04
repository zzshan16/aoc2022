#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
static inline void increment_part1(int* count, int min0, int max0, int min1, int max1){
  if (min0 < min1){
    if (max0 >= max1){
      (*count)++;
    }
  }
  else if (min0 == min1){
    (*count)++;
  }
  else{
    if (max0 <= max1){
      (*count)++;
    }
  }
}
static inline void increment_part2(int* count, int min0, int max0, int min1, int max1){
  if (min0 < min1){
    if(max0 < min1){
    }
    else{
      (*count)++;
    }
  }
  else if (min0 == min1){
    (*count)++;
  }
  else{
    if (min0 > max1){
    }
    else{
      (*count)++;
    }
  }
}
static inline int process_fun(char* map, size_t size){
  char buf[400];
  int min0, max0, min1, max1, a, count0, count1;
  size_t offset = 0;
  a = count0 = count1 = 0;
  memset(buf, 0, 400);
  while(offset<size){
    switch(map[offset]){
    case '-':
    case ',':
      a++;
      break;
    case '\n':
      min0 = atoi(buf);
      max0 = atoi(buf+100);
      min1 = atoi(buf+200);
      max1 = atoi(buf+300);
      if (a == 3){
	increment_part1(&count0, min0, max0, min1, max1);
	increment_part2(&count1, min0, max0, min1, max1);
      }
      memset(buf, 0, 400);
      a = 0;
      break;
    default:
      buf[(100 * a) + strlen(100*a + buf)] = map[offset];
      break;
    }
    offset++;
  }
  printf("part1 answer = %d\n", count0);
  printf("part2 answer = %d\n", count1);
  return 0;
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = (char*)mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  process_fun(input_map, filesize);
  munmap(input_map, filesize);
  return 0;
}
