#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
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
int check_suitable(int x, int y, char* arr){
  int pos = y*1000+x;
  /* printf("checking pos %d, %d,%d\n", pos, x,y); */
  if (y > 999) return 0;
  if (!arr[pos+1000]){
    return check_suitable(x, y+1, arr);
  }
  if (x > 0){
    if (!arr[pos+999]){
      return check_suitable(x-1, y+1, arr);
    }
  }
  if (x < 999){
    if (!arr[pos+1001]){
      return check_suitable(x+1, y+1, arr);
    }
  }
  arr[pos] = 1;
  printf("settled at %d, %d, %x.%x.%x\n", x,y,arr[pos+1000],arr[pos+1001],arr[pos+999]);
  return 1;
}
void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  char arr[1000*1000];
  memset(arr, 0, 1000000);
  int maxy = 0;
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    size_t* offset_arr = find_splits_str(buf, strlen(buf), " -> ");
    char* charptr;
    int t = 0;
    int x0;
    int y0;
    int x,y;
    x = y = y0 = x0 = 0;
    while(charptr = load_next_split(buf, offset_arr, strlen(buf))){
      //printf("%s\n", charptr);
      char* oldptr = charptr;
      char a;
      y = 0;
      x = 0;
      while(!t){
	a = *charptr++;
	switch(a){
	case '0' ... '9':
	  x0 *= 10;
	  x0 += a - '0';
	  break;
	case ',':
	  t = 1;
	  break;
	}
      }
      while(t == 1){
	a = *charptr++;
	switch(a){
	case '0' ... '9':
	  y0 *= 10;
	  y0 += a - '0';
	  break;
	case ' ':
	  t = 2;
	  goto next;
	  break;
	}
      }
      while(t == 2){
	a = *charptr++;
	switch(a){
	case '0' ... '9':
	  x *= 10;
	  x += a - '0';
	  break;
	case ',':
	  t = 3;
	  break;
	}
      }
      while(t == 3){
	a = *charptr++;
	switch(a){
	case '0' ... '9':
	  y *= 10;
	  y += a - '0';
	  break;
	case ' ':
	  t = 2;
	  break;
	}
      }
      int flag = 0;
      flag += (x-x0) ? 1 : 0;
      flag += (y-y0) ? 2 : 0;
      int sign = 0;
      switch(flag){
      case 0:
	arr[1000*y +x] = 1;
	break;
      case 1:
	sign = (x > x0) ? 1 : -1;
	for(int i = x0; i != x; i += sign){
	  arr[1000*y + i] = 1;
	}
	arr[1000*y + x] = 1;
	break;
      case 2:
	sign = (y > y0) ? 1 : -1;
	for(int i = y0; i != y; i += sign){
	  arr[1000*i + x] = 1;
	}
	arr[1000*y + x] = 1;
	break;
      case 3:
	puts("this shouldnt happen");
	printf("%d,%d -> %d,%d\n", x0,y0,x,y);
	break;
      }
      
      x0 = x;
      y0 = y;
      if (y > maxy) maxy = y;
      if (y0 > maxy) maxy = y0;
      next:;
    }
    free_ebuf();
    free(offset_arr);
  }
  maxy+=2;
  for(int i = 0; i < 1000; ++i){
    arr[maxy*1000+i] = 1;
  }
  int count = 0;
  while(!arr[500]){
    check_suitable(500, 0, arr);
    count++;
  }
  printf("%d\n", count);
  fclose(fp);
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  /* process_fun(input_map, filesize); */
  process_fgets();
  munmap(input_map, filesize);
  return 0;
}
