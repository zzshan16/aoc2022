#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include <stdint.h>

char* global_buf;
static inline void move_fun(unsigned char* dest, unsigned char* source, int num){
  /* if (*source_height - num < 0){ */
  /*   printf("would go below zero, height %d moving %d\n", *source_height, num); */
  /*   //exit(3); */
  /* } */
  uint16_t* dest_height = dest;
  uint16_t* source_height = source;
  memcpy(global_buf, source+*source_height -num +2, num);
  *source_height -= num;
  *dest_height += num;
  dest += *dest_height +2 - num;
  while (num--){
    *dest++ = global_buf[num];
  }
}
static inline void move_fun2(unsigned char* dest, unsigned char* source, int num){
  /* if (*source_height - num < 0){ */
  /*   printf("would go below zero\n"); */
  /*   //exit(3); */
  /* } */
  uint16_t* dest_height = dest;
  uint16_t* source_height = source;
  memcpy(dest + *dest_height + 2, source + *source_height - num +2, num);
  *dest_height += num;
  *source_height -= num;
}

void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  fgets(buf, 32768, fp);
  *(buf + strlen(buf) -1) = '\0';
  int len = strlen(buf);
  if (len < 4){
    printf("unexpected input\n");
    exit(2);
  }
  int num_stacks = (len&3)? (len >> 2) + 1 : len>> 2;
  unsigned char** stacks = (unsigned char**)malloc((num_stacks<<1u)*sizeof(unsigned char*));
  int t, x;
  for(int i = 0; i < (num_stacks<<1u); ++i){
    *(stacks+i) = (unsigned char*)malloc(32768);
    memset(*(stacks+i), 0, 32768);
  }
  for( int i = 1; i < len; i += 4){
    switch(buf[i]){
    case ' ':
      break;
    case '1' ... '9':
      printf("error: unexpected input");
      exit(1);
      break;
    default:;
      x = i>>2;
      t = *(uint16_t*)(stacks[x]);
      *(stacks[x] + 32768 - t) = buf[i];
      *(uint16_t*)(stacks[x]) += 1;
      *(stacks[x+num_stacks] + 32768 - t) = buf[i];
      *(uint16_t*)(stacks[x+num_stacks]) += 1;
      break;
    } 
  }  
  while(1){
    fgets(buf, 32768, fp);
    *(buf + strlen(buf) -1) = '\0';
    for(int i = 0; i < num_stacks; ++i){
      unsigned char x = *(buf+(4*i)+1);
      switch(x){
      case ' ':
	break;
      case '1' ... '9':
	goto next;
	break;
      default:
	t = *(uint16_t*)(stacks[i]);
	*(stacks[i] + 32768 - t) = x;
	*(uint16_t*)(stacks[i]) += 1;
	*(stacks[i+num_stacks] + 32768 - t) = x;
	*(uint16_t*)(stacks[i+num_stacks]) += 1;
	break;
      }
    }
  }
 next:
  for(int i = 0; i < num_stacks<<1ul; ++i){
    memmove(stacks[i] + 2, stacks[i] + 32768 - *(uint16_t*)(stacks[i])+1, *(uint16_t*)(stacks[i]));
  }
  /* for(int i = 0; i < num_stacks; ++i){ */
  /*   printf("stack %d top is %c length is %u\n", i+1, *(stacks[i]+ *(uint16_t*)(stacks[i]) +1), *(uint16_t*)(stacks[i])); */
  /*   for(int j = 0; j < *(uint16_t*)(stacks[i]); ++j){ */
  /*     printf("%c ", *(stacks[i] + j+2)); */
  /*   } */
  /* } */
  while (fgets(buf, 32768, fp)){
    int x,y,z;
    if (sscanf(buf, "move %d from %d to %d", &x,&y,&z)){
      /* printf("moving %d from %d to %d\n", x, y, z); */
      move_fun(stacks[z-1], stacks[y-1], x);
      move_fun2(stacks[z-1+num_stacks], stacks[y-1+num_stacks], x);
    }
  }
  
  /* for(int i = 0; i < num_stacks; ++i){ */
  /*   printf("stack %d top is %c\n", i+1, *(stacks[i]+ *(uint16_t*)(stacks[i]) +1)); */
  /*   for(int j = 0; j < *(uint16_t*)(stacks[i]); ++j){ */
  /*     printf("%c ", *(stacks[i] + j+2)); */
  /*   } */
  /* } */
  /* puts("\npart 2"); */
  /* for(int i = num_stacks; i < num_stacks<<1u; ++i){ */
  /*   printf("stack %d top is %c\n", i+1, *(stacks[i]+ *(uint16_t*)(stacks[i])+1)); */
  /*   for(int j = 0; j < *(uint16_t*)(stacks[i]); ++j){ */
  /*     printf("%c ", *(stacks[i] + j+2)); */
  /*   } */
  /* } */
  /* puts(""); */
  printf("part1: ");
  for(int i = 0; i < num_stacks; ++i){
    printf("%c", *(stacks[i]+ *(uint16_t*)(stacks[i]) +1));
  }
  printf("\npart2: ");
  for(int i = num_stacks; i < num_stacks << 1u; ++i){
    printf("%c", *(stacks[i]+ *(uint16_t*)(stacks[i])+ 1));
  }
  printf("\n");
  fclose(fp);
}

int main(int argc, char** argv){
  global_buf = malloc(50000);
  process_fgets();
  free(global_buf);
  return 0;
}
