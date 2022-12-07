#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include <stdint.h>
#define WINDOW_SIZE 14

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  char mapping[128];
  memset(mapping, 0, 128);
  char moved = 0;
  char* beginp = input_map;
  char* nextp = input_map + WINDOW_SIZE;
  char remaining = WINDOW_SIZE;
 do_remaining:
  while (remaining){
    mapping[*(nextp - remaining--)]++;
    moved++;
  }
  int t = moved;
  while(moved){
    if (mapping[*(nextp - moved)] == 1){
      t--;
    }
    moved--;
  }
  if (!t){
    printf("found soln %ld\n", nextp - input_map);
  }
 loopback:
  //remaining = moved = 0;
  mapping[*beginp++]--;
  mapping[*nextp]++;
  if (mapping[*nextp++] == 1){
    for(unsigned int i = 0x20; i < 0x7E; i+=8){
      if( (*(uint64_t*)(mapping+i)) & 0xFEFEFEFEFEFEFEFE){
	goto loopback;
      }
    }
    printf("done %ld\n", nextp - input_map);
    goto done;
  }
  goto loopback;
 done:
  munmap(input_map, filesize);
  return 0;
}
