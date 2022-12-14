#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>
#include "simple.h"

int distance = 0;
typedef struct node{
  int branches;
  signed char height;
  int dist;
  int pos;
} node;
void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  char a[161*41+1];
  a[161*41] = 0;
  /* node* nodeptr[161*41]; */
  /* memset(nodeptr, 0, 161*41*sizeof(node*)); */
  int row = 0;
  node n_a[161*41];
  for(int i = 0; i < 161*41; ++i){
    n_a[i].dist = 0;
  }
  ssvect stacks[2];
  if (!ssvect_init(stacks, 64*sizeof(void*), sizeof(void*))){
    puts("failed to create stack");
    return;
  }
  if (!ssvect_init(stacks+1, 64*sizeof(void*), sizeof(void*))){
    puts("failed to create stack");
    return;
  }
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    memcpy(a+row++*161, buf, 161);
    /* printf("%d\n", strlen(buf)); */
  }
  node* current_position = 0;
  for(int i = 0; i < 161*41; ++i){
    if (a[i] == 'E'){
      current_position = n_a + i;
      current_position->height = 'z';
      current_position->dist = -1;
      current_position->pos = i;
      //printf("current position is %d\n", i);
      break;
    }
  }
  /* for(int i = 0; i < 161*41; ++i){ */
  /*   if (i % 161 == 0) putchar('\n'); */
  /*   putchar(a[i]); */
  /* } */
  /* printf("%s", a); */
  CF(stacks, push, &current_position);
  ssvect* stack1 = stacks;
  ssvect** stackc = &stack1;
  ssvect* stack2 = stacks+1;
  ssvect** stackn = &stack2;
  void* stackt;
  uint32_t loop_count = 0;
  int adj;
  int x;
  int y;
  node* np;
 loopback:
  ++distance;
  while(stackt = CF(*stackc,pop)){
    current_position = *(node**)stackt;
    printf("evaluating position %d\n", current_position->pos);
    x = current_position->pos % 161;
    y = current_position->pos / 161;
    printf("x = %d, y = %d\n", x, y);
    if (y > 0){//consider neighbor above
      adj = x + 161*(y-1);
      if (!n_a[adj].dist){
	if (a[adj] == 'a'){
	  if (current_position->height <= 'b'){
	    printf("found end at distance %d\n", distance);
	    goto end;
	  }
	}
	if(current_position->height - a[adj] <= 1){
	  n_a[adj].dist = distance;
	  n_a[adj].height = a[adj];
	  n_a[adj].pos = adj;
	  np = n_a+adj;
	  CF(*stackn,push,&np);
	}
      }
    }
    if (x > 0){//consider neighbor to the left
      adj = (x-1) + 161*y;
      if (!n_a[adj].dist){
	if (a[adj] == 'a'){
	  if (current_position->height == 'b'){
	    printf("found end at distance %d\n", distance);
	    goto end;
	  }
	}
	if(current_position->height - a[adj] <= 1){
	  n_a[adj].dist = distance;
	  n_a[adj].height = a[adj];
	  n_a[adj].pos = adj;
	  np = n_a+adj;
	  CF(*stackn,push,&np);
	  //CF(stackn,push,n_a+adj);
	}
      }
    }
    if (y < 40){//consider neighbor below
      adj = x + 161*(y+1);
      if (!n_a[adj].dist){
	if (a[adj] == 'a'){
	  if (current_position->height == 'b'){
	    printf("found end at distance %d\n", distance);
	    goto end;
	  }
	}
	if(current_position->height -a[adj] <= 1){
	  n_a[adj].dist = distance;
	  n_a[adj].height = a[adj];
	  n_a[adj].pos = adj;
	  np = n_a+adj;
	  CF(*stackn,push,&np);
	  //CF(stackn,push,n_a+adj);
	}
      }
    }
    if (x < 160){//consider neighbor to the left
      adj = (x+1) + 161*y;
      if (!n_a[adj].dist){
	if (a[adj] == 'a'){
	  if (current_position->height == 'b'){
	    printf("found end at distance %d\n", distance);
	    goto end;
	  }
	}
	if(current_position->height - a[adj] <= 1){
	  n_a[adj].dist = distance;
	  n_a[adj].height = a[adj];
	  n_a[adj].pos = adj;
	  np = n_a+adj;
	  CF(*stackn,push,&np);
	  //CF(stackn,push,n_a+adj);
	}
      }
    }
    printf("next size is %d\n", (*stackn)->size / (*stackn)->element_size);
    for(int i = 0; i < (*stackn)->size; i+= (*stackn)->element_size){
      printf("%d: %d \n", i, (*(node**)((*stackn)->head+i))-> pos);
    }
  }
  //if (++loop_count > 1000000000) goto end;
  puts("swapping");
  stackt = *stackc;
  *stackc = *stackn;
  *stackn = stackt;
  printf("next size is %d\n", (*stackc)->size / (*stackc)->element_size);
  goto loopback;
 end:
  fclose(fp);
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  process_fgets();
  munmap(input_map, filesize);
  return 0;
}
