#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static inline int process_fun(char* map, size_t size){
  int a, count0, count1;
  int nums[4];
  size_t offset = 0;
  a = count0 = count1 = 0;
  memset(nums,0,4*sizeof(int));
  while(offset<size){
    switch(map[offset]){
    case '-':
    case ',':
      a++;
      break;
    case '\n':
      if ((nums[0]-nums[2]) * (nums[1]-nums[3]) <= 0) count0++;
      if ((nums[0]-nums[3]) * (nums[1]-nums[2]) <= 0) count1++;
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
