#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

long int process_fun(char* map, size_t size){
  int i = 0;
  long int max = 0;
  long int max2 = 0;
  long int max3 = 0;
  char* ptr = map;
  int x = 0;
  long int current = 0;
  int z = 1;
  x = 0;
  while (ptr != map+size-1){
    int y;
    switch(*ptr++){
    case '0':
      y = 0;
      break;
    case '1':
      y = 1;
      break;
    case '2':
      y = 2;
      break;
    case '3':
      y = 3;
      break;
    case '4':
      y = 4;
      break;
    case '5':
      y = 5;
      break;
    case '6':
      y = 6;
      break;
    case '7':
      y = 7;
      break;
    case '8':
      y = 8;
      break;
    case '9':
      y = 9;
      break;
    case '\n':
      y = -1;
      --z;
      break;
    default:
      printf("got unexpected char %x\n", *(ptr-1));
      y = -2;
      break;
    }
    if (y == -2){
      continue;
    }
    if (y == -1){
      //puts("newline");
      if (z == 0){
	current += x;
	x = 0;
	//printf("current = %d\n", current);
      }
      if (z < 0){
	if (current > max){
	  max3 = max2;
	  max2 = max;
	  max = current;
	  //printf("new max = %ld\n", max);
	}
	else if (current > max2){
	  max3 = max2;
	  max2 = current;
	}
	else if (current > max3){
	  max3 = current;
	}
	current = 0;
	x = 0;
	z = 1;
      }
      continue;
    }
    x *= 10;
    x += y;
    z = 1;
    //printf("x = %d\n", x);
  }
  printf("max = %ld, sum of 3 highest = %ld\n", max, max+max2+max3);
  return max;
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = (char*)mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  
  long int ans = process_fun(input_map, filesize);
  
  munmap(input_map, filesize);
  return 0;
}
