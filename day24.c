#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "simple.h"

typedef struct obj{
  char type;
  int x;
  int y;  
} obj;


uint64_t u64(uint32_t x, uint32_t y){
  uint64_t ret = y;
  return (ret << 32) | x;
}
int max_x;
int max_y;
int first_empty_slot(obj** arr, int x, int y){
  obj* op;
  for(int i = 0; i < 8; ++i){
    op = arr[y*8000+x*8+i];
    if (!op) return i;
  }
  return -1;
}
int first_nonempty_slot(obj** arr, int x, int y){
  obj* op;
  for(int i = 0; i < 8; ++i){
    op = arr[y*8000+x*8+i];
    if (op) return i;
  }
  return -1;
}
int filled_slots(obj** arr, int x, int y){
  int count = 0;
  for(int i = 0; i < 8; ++i){
    if (arr[y*8000+x*8+i]) count++;
  }
  return count;
}  
int find_current_slot(obj** arr, obj* o){
  obj* op;
  for(int i = 0; i < 8; ++i){
    op = arr[o->y*8000+o->x*8+i];
    if (op == o) return i;
  }
  return -1;
}
void move_objs(obj** arr, obj* objs, int num_objs){
  for(int i = 0; i < num_objs;++i){
    obj* op = objs+i;
    switch(op->type){
    case '#':
      break;
    case '<':
      arr[op->y*8000 + op->x*8 + find_current_slot(arr, op)] = 0;
      if (op->x == 1){
	op->x = max_x-1;
      }
      else{
	(op->x)--;
      }
      arr[(op->y*8000) + (op->x*8) + first_empty_slot(arr, op->x, op->y)] = op;
      /* printf("find current slot %d\n", find_current_slot(arr, op)); */
      break;
    case '^':
      arr[op->y*8000 + op->x*8 + find_current_slot(arr, op)] = 0;
      if (op->x == 1 && op->y == 0){
	op->y = max_y-1;
      }
      else if (op->x == max_x-1 && op->y == 1){
	op->y = max_y;
      }
      else if (op->y == 1){
	op->y = max_y-1;
      }
      else{
	(op->y)--;
      }
      arr[op->y*8000 + op->x*8 + first_empty_slot(arr, op->x, op->y)] = op;
      break;
    case '>':
      arr[op->y*8000 + op->x*8 + find_current_slot(arr, op)] = 0;
      if (op->x == max_x-1){
	op->x = 1;
      }
      else{
	(op->x)++;
      }
      arr[op->y*8000 + op->x*8 + first_empty_slot(arr, op->x, op->y)] = op;
      break;
    case 'v':
      arr[op->y*8000 + op->x*8 + find_current_slot(arr, op)] = 0;
      if (op->x == 1 && op->y == max_y -1){
	op->y = 0;
      }
      else if (op->x == max_x-1 && op->y == max_y){
	op->y = 1;
      }
      else if (op->y == max_y -1){
	op->y = 1;
      }
      else{
	(op->y)++;
      }
      arr[op->y*8000 + op->x*8 + first_empty_slot(arr, op->x, op->y)] = op;
      break;
    }
  }
}

uint16_t small_pos(uint64_t pos){
  uint16_t ret = pos & 0xFF;
  return (ret << 8) | ((pos >> 32) & 0xFF);
}

int goal0;
int goal1;
int prune_options(ssvect* current, ssvect* next){
  //prunes duplicate positions
  uint64_t pos;
  uint16_t spos;
  uint64_t *table = malloc(3*0xFFFF*sizeof(uint64_t));
  memset(table, 0, 3*0xFFFF*sizeof(uint64_t));
  uint64_t* posp;
  while(posp = CF(current, pop)){
    pos = *posp;
    spos = small_pos(pos);
    if (!table[3*spos]){
      table[3*spos] = pos;
      continue;
    }
    else if (table[3*spos] == pos) {
      continue;
    }
    else if (!table[3*spos+1]){
      table[3*spos+1] = pos;
      continue;
    }
    else if (table[3*spos+1] == pos) {
      continue;
    }
    else if (!table[3*spos+2]){
      table[3*spos+2] = pos;
      continue;
    }
    printf("more than 3 with same modulo pos, %x %x %x\n", table[3*spos], table[3*spos+1], table[3*spos+2]);
  }
  for(int i = 0; i < 3*0xFFFF; ++i){
    if (table[i]){
      CF(next,push,table+i);
    }
  }
  free(table);
  return 0;
}
int explore_options(ssvect* current, ssvect* next, obj** arr){
  int count = 0;
  uint64_t pos;
  uint64_t* posp;
  int32_t x,y;
  /* printf("stack size %d\n", current->size/current->element_size); */
  while(posp = CF(current, pop)){
    //do 4 directions, adding any that work. if all fail then nothing will go to the next stack
    //also check current position
    //prune after X iterations
    pos = *posp;
    /* free(*(uint64_t**)posp); */
    /* printf("current pos = %x\n", pos); */
    x = (uint32_t) (pos & 0xFFFFFFFF);
    y = (uint32_t) (pos >> 32);
    if(!filled_slots(arr, x, y)){
      posp = malloc(sizeof(uint64_t));
      CF(next, push, &pos);
      count++;
    }
    if(!filled_slots(arr, x-1, y)){
      if (x-1 >= 0){
	pos = u64(x-1, y);
	CF(next, push, &pos);
	count++;
      }
    }
    if(!filled_slots(arr, x+1, y)){
      if (x+1 <= max_x){
	pos = u64(x+1, y);
	/* posp = malloc(sizeof(uint64_t)); */
	/* *posp = pos; */
	CF(next, push, &pos);
	count++;
      }
    }
    if(!filled_slots(arr, x, y-1)){
      if (y-1 == 0){
	goal1 = 1;
      }
      if (y-1 > 0){
	pos = u64(x, y-1);
	CF(next, push, &pos);
	count++;
      }
    }
    if(!filled_slots(arr, x, y+1)){
      /* puts("here"); */
      if (y+1 == max_y){
	goal0 = 1;
      }
      else if (y+1 < max_y){
	pos = u64(x, y+1);
	CF(next, push, &pos);
	count++;
      }
    }    
  }
  return count;
}
void print_status(obj** arr){  
  for( int i = 0; i < 10; ++i){
    for(int j = 0; j < 10; ++j){
      int s = filled_slots(arr, j, i);
      switch(s){
      case 0:
	printf(".");
	break;
      case 1:
	printf("%c", arr[i*8000+j*8+first_nonempty_slot(arr, j, i)]->type);
	break;
      default:
	printf("%d", s);
      }
    }
    printf("\n");
  }
}
int process_fun(char* map, size_t size){  
  obj** arr = malloc(1000*1000*8*sizeof(obj*));
  obj* obj_arr = malloc(1000*1000*sizeof(obj));
  memset(arr, 0, 1000*1000*8*sizeof(obj*));
  if (!arr || ! obj_arr){
    puts("malloc");
    exit(2);
  }
  int32_t x = 0;
  int32_t y = 0;
  char* cptr = map;
  int num_objs = 0;
  /* printf("sizeof void* = %ld\n", sizeof(void*)); */
  while(cptr - map < size){
    switch(*cptr){
    case '#':
      obj_arr[num_objs].x = x;
      obj_arr[num_objs].y = y;
      obj_arr[num_objs].type = '#';
      arr[y*8000+x*8] = obj_arr+num_objs;
      num_objs++;
      x++;
      break;
    case '.':
      x++;
      break;
    case '\n':
      x = 0;
      y++;
      break;
    case '<':
      obj_arr[num_objs].x = x;
      obj_arr[num_objs].y = y;
      obj_arr[num_objs].type = '<';
      arr[y*8000+x*8] = obj_arr+num_objs;
      num_objs++;
      x++;
      break;
    case '^':
      obj_arr[num_objs].x = x;
      obj_arr[num_objs].y = y;
      obj_arr[num_objs].type = '^';
      arr[y*8000+x*8] = obj_arr+num_objs;
      num_objs++;
      x++;
      break;
    case '>':
      obj_arr[num_objs].x = x;
      obj_arr[num_objs].y = y;
      obj_arr[num_objs].type = '>';
      arr[y*8000+x*8] = obj_arr+num_objs;
      num_objs++;
      x++;
      break;
    case 'v':
      obj_arr[num_objs].x = x;
      obj_arr[num_objs].y = y;
      obj_arr[num_objs].type = 'v';
      arr[y*8000+x*8] = obj_arr+num_objs;
      num_objs++;
      x++;
      break;
    }
    cptr++;
  }
  //movement of the blizzard is constant, the only variable is where the person moves
  x = 1;
  y = 0;
  max_x = 0;
  max_y = 0;
  for (int i = 0; i <num_objs; ++i){
    if (obj_arr[i].x > max_x) max_x = obj_arr[i].x;
    if (obj_arr[i].y > max_y) max_y = obj_arr[i].y;
  }
  printf("max_x %d max_y %d\n", max_x, max_y);
  uint64_t pos = u64(x,y);
  ssvect stack[2];
  ssvect* stackp = stack;
  ssvect* stackn = stack+1;
  ssvect_init(stackp, 100*sizeof(uint64_t), sizeof(uint64_t));
  ssvect_init(stackn, 100*sizeof(uint64_t), sizeof(uint64_t));
  CF(stackp, push, &pos);
  int t = 0;
  ssvect* temp;
  goal0 = 0;
  while(t < 10000){
    move_objs(arr, obj_arr, num_objs);
    /* print_status(arr); */
    explore_options(stackp, stackn, arr);
    temp = stackp;
    stackp = stackn;
    stackn = temp;
    t++;
    if (!(t&1)){
      prune_options(stackp, stackn);
      temp = stackp;
      stackp = stackn;
      stackn = temp;
    }
    if (goal0){
      printf("part 1: %d\n", t);
      goto part2;
    }
  }
 part2:
  while(CF(stackp, pop));
  while(CF(stackn, pop));
  pos = u64(max_x-1,max_y);
  CF(stackp, push, &pos);
  goal1 = 0;
  while(t < 10000){
    move_objs(arr, obj_arr, num_objs);
    /* print_status(arr); */
    explore_options(stackp, stackn, arr);
    temp = stackp;
    stackp = stackn;
    stackn = temp;
    t++;
    if (!(t & 1)){
      prune_options(stackp, stackn);
      temp = stackp;
      stackp = stackn;
      stackn = temp;
    }
    if (goal1){
      printf("back to start: %d\n", t);
      goto part3;
    }
  }  
 part3:
  while(CF(stackp, pop));
  while(CF(stackn, pop));
  pos = u64(1,0);
  CF(stackp, push, &pos);
  goal0 = 0;
  while(t < 10000){
    move_objs(arr, obj_arr, num_objs);
    /* print_status(arr); */
    explore_options(stackp, stackn, arr);
    temp = stackp;
    stackp = stackn;
    stackn = temp;
    t++;
    if (!(t&1)){
      prune_options(stackp, stackn);
      temp = stackp;
      stackp = stackn;
      stackn = temp;
    }
    if (goal0){
      goto done;
    }
  }  
 done:
  printf("part 2: %d\n", t);
  free(stackp->head);
  free(stackn->head);  
  free(arr);
  free(obj_arr);
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
  /* process_fgets(); */
  munmap(input_map, filesize);
  return 0;
}
