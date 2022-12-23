#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "simple.h"
#define MAX_X 10000
#define MAX_Y 10000
#define MIN_X 1000
#define MIN_Y 1000

int facing;
int moved;
static inline void cycle_facing(){
  facing = (facing + 1) % 4;//NSWE
}

typedef struct elf{
  int x;
  int y;
  int new_x;
  int new_y;
} elf;


static inline int check_dir(elf** arr, int x, int y, int dir){
  switch(dir){
  case 0:
    if (!arr[(y-1)*MAX_X+x+1] && !arr[(y-1)*MAX_X+x+0] && !arr[(y-1)*MAX_X+x-1])
      return 1;
    return 0;
  case 1:
    if (!arr[(y+1)*MAX_X+x+1] && !arr[(y+1)*MAX_X+x+0] && !arr[(y+1)*MAX_X+x-1])
      return 1;
    return 0;
  case 2:
    if (!arr[(y+1)*MAX_X+x-1] && !arr[(y+0)*MAX_X+x-1] && !arr[(y-1)*MAX_X+x-1])
      return 1;
    return 0;
  case 3:
    if (!arr[(y+1)*MAX_X+x+1] && !arr[(y+0)*MAX_X+x+1] && !arr[(y-1)*MAX_X+x+1])
      return 1;
    return 0;
  }
}
static inline void move_elf(elf* e, elf** arr){
  int num = 0;
  int x = e->new_x;
  int y = e->new_y;
  if (!x || !y) return;
  elf* elfp;
  /* puts("here"); */
  if (elfp = arr[(y-1)*MAX_X+x+0]){
    if (elfp->new_x == x && elfp->new_y == y){
      num++;
    }
  }
  if (elfp = arr[(y+1)*MAX_X+x+0]){
    if (elfp->new_x == x && elfp->new_y == y){
      num++;
    }
  }
  if (elfp = arr[(y)*MAX_X+x-1]){
    if (elfp->new_x == x && elfp->new_y == y){
      num++;
    }
  }
  if (elfp = arr[(y)*MAX_X+x+1]){
    if (elfp->new_x == x && elfp->new_y == y){
      num++;
    }
  }
  if (num == 1){
    arr[y*MAX_X + x] = e;
    arr[e->y * MAX_X + e->x] = 0;
    e->y = y;
    e->x = x;
    e->new_x = 0;
    e->new_y = 0;
    moved = 1;
  }
}
static inline int neighbors(int x, int y, elf** arr){
  if (arr[MAX_X*(y-1) + x]) return 1;
  if (arr[MAX_X*(y-1) + x-1]) return 1;
  if (arr[MAX_X*(y-1) + x+1]) return 1;
  if (arr[MAX_X*(y+1) + x]) return 1;
  if (arr[MAX_X*(y+1) + x-1]) return 1;
  if (arr[MAX_X*(y+1) + x+1]) return 1;
  if (arr[MAX_X*y + x-1]) return 1;
  if (arr[MAX_X*y + x+1]) return 1;
  return 0;
}
static inline void find_new(elf* e, elf** arr){
  int x = e->x;
  int y = e->y;
  if (!neighbors(x,y,arr)){
    e->new_x = 0;
    e->new_y = 0;
    return;
  }
  switch(facing){
  case 0:
    //check north, south, west, east
    if (check_dir(arr, x, y, 0)){
      e->new_x = x;
      e->new_y = y-1;
    }
    else if (check_dir(arr, x, y, 1)){
      e->new_x = x;
      e->new_y = y+1;
    }
    else if (check_dir(arr, x, y, 2)){
      e->new_x = x-1;
      e->new_y = y;
    }
    else if (check_dir(arr, x, y, 3)){
      e->new_x = x+1;
      e->new_y = y;
    }
    else{
      e->new_x = 0;
      e->new_y = 0;
    }
    break;
  case 1:
    if (check_dir(arr, x, y, 1)){
      e->new_x = x;
      e->new_y = y+1;
    }
    else if (check_dir(arr, x, y, 2)){
      e->new_x = x-1;
      e->new_y = y;
    }
    else if (check_dir(arr, x, y, 3)){
      e->new_x = x+1;
      e->new_y = y;
    }
    else if (check_dir(arr, x, y, 0)){
      e->new_x = x;
      e->new_y = y-1;
    }
    else{
      e->new_x = 0;
      e->new_y = 0;
    }    
    break;
  case 2:
    if (check_dir(arr, x, y, 2)){
      e->new_x = x-1;
      e->new_y = y;
    }
    else if (check_dir(arr, x, y, 3)){
      e->new_x = x+1;
      e->new_y = y;
    }
    else if (check_dir(arr, x, y, 0)){
      e->new_x = x;
      e->new_y = y-1;
    }
    else if (check_dir(arr, x, y, 1)){
      e->new_x = x;
      e->new_y = y+1;
    }
    else{
      e->new_x = 0;
      e->new_y = 0;
    }    
    break;
  case 3:
    if (check_dir(arr, x, y, 3)){
      e->new_x = x+1;
      e->new_y = y;
    }
    else if (check_dir(arr, x, y, 0)){
      e->new_x = x;
      e->new_y = y-1;
    }
    else if (check_dir(arr, x, y, 1)){
      e->new_x = x;
      e->new_y = y+1;
    }
    else if (check_dir(arr, x, y, 2)){
      e->new_x = x-1;
      e->new_y = y;
    }    
    else{
      e->new_x = 0;
      e->new_y = 0;
    }    
    break;
  }
}

int process_fun(char* map, size_t size){
  /* size_t* offset_arr = find_splits_str(map, size, "\n"); */
  char* charptr = map;
  /* if (!offset_arr){puts("something wrong"); exit(1);} */
  elf **arr = malloc(MAX_X * MAX_Y * sizeof(elf*));
  elf *elf_arr = malloc(MAX_X * MAX_Y *sizeof(elf));
  if (!arr || !elf_arr){
    puts("malloc failed");
    exit(5);
  }
  memset(elf_arr, 0, MAX_X * MAX_Y *sizeof(elf));
  memset(arr,0,MAX_X * MAX_Y * sizeof(elf*));
  int x = MIN_X;
  int y = MIN_Y;
  int num_elves = 0;
  elf *elfp;
  while(charptr-map < size){
    switch(*charptr){
    case '.':
      x++;
      break;
    case '\n':
      x = MIN_X;
      y++;
      break;
    case '#':
      arr[y*MAX_X + x] = elf_arr + num_elves;
      elfp = elf_arr+num_elves;
      elfp->x = x;
      elfp->y = y;
      elfp->new_x = 0;
      elfp->new_y = 0;
      num_elves++;
      x++;
      break;
    }
    charptr++;
  }
  printf("num elves %d\n", num_elves);
  int t = 0;
  int x0, x1, y0, y1;

  while(t++ < 10){
    for (int i = 0; i < num_elves; ++i){
      find_new(elf_arr+i, arr);
    }
    for (int i = 0; i < num_elves; ++i){
      move_elf(elf_arr+i, arr);
    }
    cycle_facing();
    /* x0 = x1 = elf_arr->x; */
    /* y0 = y1 = elf_arr->y; */
    /* for (int i = 1; i < num_elves; ++i){ */
    /*   if (elf_arr[i].x > x1){ */
    /* 	x1 = elf_arr[i].x; */
    /*   } */
    /*   else if (elf_arr[i].x < x0){ */
    /* 	x0 = elf_arr[i].x; */
    /*   } */
    /*   if (elf_arr[i].y > y1){ */
    /* 	y1 = elf_arr[i].y; */
    /*   } */
    /*   else if (elf_arr[i].y < y0){ */
    /* 	y0 = elf_arr[i].y; */
    /*   } */
    /* } */
    /* printf("end of round %d:\n", t); */
    /* for(int j = y0; j <= y1; ++j){ */
    /*   for(int i = x0; i <= x1; ++i){ */
    /* 	if (!arr[j*MAX_X + i]){ */
    /* 	  printf("."); */
    /* 	} */
    /* 	else{ printf("#"); */
    /* 	} */
    /*   } */
    /*   printf("\n"); */
    /* } */

  }
  x0 = x1 = elf_arr->x;
  y0 = y1 = elf_arr->y;
  for (int i = 1; i < num_elves; ++i){
    if (elf_arr[i].x > x1){
      x1 = elf_arr[i].x;
    }
    else if (elf_arr[i].x < x0){
      x0 = elf_arr[i].x;
    }
    if (elf_arr[i].y > y1){
      y1 = elf_arr[i].y;
    }
    else if (elf_arr[i].y < y0){
      y0 = elf_arr[i].y;
    }
  }
  int count = 0;
  for(int i = x0; i <= x1; ++i){
    for(int j = y0; j <= y1; ++j){
      if (!arr[j*MAX_X + i])
	count++;
    }
  }
  printf("part1 = %d\n", count);
  moved = 1;
  while(moved){
    moved = 0;
    for (int i = 0; i < num_elves; ++i){
      find_new(elf_arr+i, arr);
    }
    for (int i = 0; i < num_elves; ++i){
      move_elf(elf_arr+i, arr);
    }
    cycle_facing();
    t++;
  }
  printf("part2 = %d\n", t-1);
  /* for(int j = y0; j <= y1; ++j){ */
  /*   for(int i = x0; i <= x1; ++i){ */
  /*     if (!arr[j*MAX_X + i]){ */
  /* 	printf("."); */
  /*     } */
  /*     else{ printf("#"); */
  /*     } */
  /*   } */
  /*   printf("\n"); */
  /* } */
  facing = 0;
  free(arr);
  free(elf_arr);
  free_ebuf();
  return 0;
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  process_fun(input_map, filesize);
  munmap(input_map, filesize);
  return 0;
}
