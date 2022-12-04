#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
static inline void increment_counts(int* count0, int* count1, int min0, int max0, int min1, int max1){
  if (min0 > max1 || min1 > max0) return;
  if ((max0 >= max1 && min0 <= min1) || (max0 <= max1 && min0 >= min1))
    (*count0)++;
  (*count1)++;
}
static inline int process_fun(char* map, size_t size){
  int a, count0, count1;
  int nums[4];
  size_t offset = 0;
  a = count0 = count1 = 0;
  while(offset<size){
    switch(map[offset]){
    case '-':
    case ',':
      a++;
      break;
    case '\n':
      if (a == 3){
	increment_counts(&count0, &count1, nums[0], nums[1], nums[2], nums[3]);
      }
      a = 0;
      memset(nums, 0, 4*sizeof(int));
      break;
    default:
      nums[a] *= 10;
      nums[a] += map[offset] - '0';
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
