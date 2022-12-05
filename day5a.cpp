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
static inline void move_fun(char* dest, char* source){
  //int source_height = *source;
  dest[++*dest] = source[(*source)--];
}
int process_fun(char* map, int size){
  size_t* offset_arr = find_splits_str(map, size, "\n");
  char* charptr;
  if (!offset_arr){puts("something wrong"); exit(1);}
  int t = 0;
  char** stacks = (char**)malloc(9*sizeof(char*));
  for(int i = 0; i < 9; ++i){
    *(stacks+i) = (char*)malloc(1024);
    memset(*(stacks+i), 0, 1024);
  }
  while (strlen(charptr = load_next_split(map,offset_arr,size)) == 36){
    //printf("%s\n", charptr);
    for(int i = 0; i < 9; ++i){
      char x = *(charptr+(4*i)+1);
      switch(x){
      case ' ':
	break;
      case '1' ... '9':
	goto next;
	break;
      default:
	t = *(stacks[i]);
	*(*(stacks+i) + 1023 - t) = x;
	*(stacks[i]) += 1;
	break;
      }
    }
  }
 next:
  
  for(int i = 0; i < 9; ++i){
    size_t len = *(stacks[i]);
    memmove(stacks[i] + 1, stacks[i] + 1024 - len, len);
  }
  while (strlen(charptr = load_next_split(map,offset_arr,size))){
    int x,y,z;
    if (sscanf(charptr, "move %d from %d to %d", &x,&y,&z)){
      printf("moving %d from %d to %d\n", x, y, z);
      while (x--){
	move_fun(stacks[z-1], stacks[y-1]);
      }
    }
  }
  for(int i = 0; i < 9; ++i){
    printf("stack %d top is %c\n", i+1, *(stacks[i]+ *(stacks[i])));
  }
  for(int i = 0; i < 9; ++i){
    printf("%c", *(stacks[i]+ *(stacks[i])));
  }
  free_ebuf();
  free(offset_arr);
  return 0;
}

void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
  }
  
  fclose(fp);
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = (char*)mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  
  process_fun(input_map, filesize);
  process_fgets();
  munmap(input_map, filesize);
  return 0;
}
