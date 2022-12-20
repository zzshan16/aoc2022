#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "simple.h"
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
typedef struct state{
  char ore_r;
  char clay_r;
  char obs_r;
  char geo_r;
  char ore_rm;
  char ore;
  char clay;
  char clay_rm;
  char obs;
  char geo;
  char cost[6];
  char option;
  char path[30];
  char time;
} state;

static inline state* branch_state(state* x, int option){
  state* new_state = malloc(sizeof(state));
  if (!new_state){
    puts("malloc failed");
    exit(5);
  }
  memcpy(new_state, x, sizeof(state));
  new_state->option = option;
  return new_state;
}


state** astates;
int nstates;
int max_geo;
void update_time(state* x){
  x->ore += x->ore_r;
  x->clay += x->clay_r;
  x->obs += x->obs_r;
  x->geo += x->geo_r;
  x->time++;
}
int min(int a, int b){
  return (a < b) ? a : b;
}

void compare_max(state* current){
  /* puts("here"); */
  if (current->geo > max_geo){
    max_geo = current->geo;
    printf("Path found producing %d geodes: ", current->geo);	  
    for(int i = 0; i < 30; ++i){
      if (current->path[i]){
	printf("%d %c,", i+1, current->path[i]);
      }
    }
    printf("\n");
  }
}
void print_path(state* current){
  return;
  printf("examining build order: ");
  for(int i = 0; i < 30; ++i){
    if (current->path[i]){
      printf("%d %c, ", i+1, current->path[i]);
    }
  }  
  printf(" ??%d \n", current->option);
}
int time_limit;
int dev_states(ssvect* stackp, ssvect* stackn){
  state* current;
  state* next;
  int num = 0;
  printf("current stack size %ld\n", stackp->size/stackp->element_size);
  while(current = CF(stackp, pop)){
    ++num;
    /* printf("2 %p\n", current); */
    current = *(state**)current;
    /* if (current->time > 28){ */
    /*   printf("current time %d\n", current->time); */
    /* } */
    /* printf("3 %p\n", current); */
    /* print_path(current); */
    /* printf("current time is %d, option is %d\n", current->time, current->option); */
    switch(current->option){
    case 0:
      if(current->time < time_limit && current->ore < current->cost[0]){
	update_time(current);
	CF(stackn, push, &current);
	continue;
      }
      if (current->time >= time_limit){
	compare_max(current);
	goto check_next;
      }
      current->path[current->time] = 'R';
      update_time(current);
      current->ore -= current->cost[0];
      current->ore_r++;
      if (current->time >= time_limit){
	compare_max(current);
	goto check_next;
      }
      if (current->ore_r < current->ore_rm){
	next = branch_state(current, 0);
	CF(stackn, push, &next);
      }
      if (current->clay_r < current->clay_rm){
	next = branch_state(current, 1);
	CF(stackn, push, &next);
      }
      if (current->clay_r > 0){
	next = branch_state(current, 2);
	CF(stackn, push, &next);
      }
      if (current->obs_r){
	next = branch_state(current, 3);
	CF(stackn, push, &next);
      }
      break;
    case 1:
      if(current->time < time_limit && current->ore < current->cost[1]){
	update_time(current);
	CF(stackn, push, &current);
	continue;
      }
      if (current->time >= time_limit){
	compare_max(current);
	goto check_next;
      }
      current->path[current->time] = 'C';
      update_time(current);
      current->ore -= current->cost[1];
      current->clay_r++;
      if (current->time >= time_limit){
	compare_max(current);
	goto check_next;
      }
      if (current->ore_r < current->ore_rm){
	next = branch_state(current, 0);
	CF(stackn, push, &next);
      }
      if (current->clay_r < current->clay_rm){
	next = branch_state(current, 1);
	CF(stackn, push, &next);
      }
      next = branch_state(current, 2);
      CF(stackn, push, &next);
      if (current->obs_r){
	next = branch_state(current, 3);
	CF(stackn, push, &next);
      }
      
      break;
    case 2:
      if(current->time < time_limit && (current->ore < current->cost[2] || current->clay < current->cost[3])){
	update_time(current);
	CF(stackn, push, &current);
	continue;
      }
      if (current->time >= time_limit){
	compare_max(current);
	goto check_next;
      }
      current->path[current->time] = 'S';
      update_time(current);
      current->ore -= current->cost[2];
      current->clay -= current->cost[3];
      current->obs_r++;
      if (current->time >= time_limit){
	compare_max(current);
	goto check_next;
      }
      if (current->ore_r < current->ore_rm){
	next = branch_state(current, 0);
	CF(stackn, push, &next);
      }
      if (current->clay_r < current->clay_rm){
	next = branch_state(current, 1);
	CF(stackn, push, &next);
      }
      next = branch_state(current, 2);
      CF(stackn, push, &next);
      next = branch_state(current, 3);
      CF(stackn, push, &next);
      
      break;
    case 3:
      if(current->time < time_limit && (current->ore < current->cost[4] || current->obs < current->cost[5])){
	update_time(current);
	CF(stackn, push, &current);
	continue;
      }
      if (current->time >= time_limit){
	compare_max(current);
	goto check_next;
      }
      current->path[current->time] = 'G';
      update_time(current);
      current->ore -= current->cost[4];
      current->obs -= current->cost[5];
      current->geo_r++;
      if (current->time >= time_limit){
	compare_max(current);
	goto check_next;
      }
      if (current->ore_r < current->ore_rm){
	next = branch_state(current, 0);
	CF(stackn, push, &next);
      }
      if (current->clay_r < current->clay_rm){
	next = branch_state(current, 1);
	CF(stackn, push, &next);
      }
      next = branch_state(current, 2);
      CF(stackn, push, &next);
      next = branch_state(current, 3);
      CF(stackn, push, &next);
      
      break;
    default:
      puts("error");
      
      break;
    }
    /* update_time(current); */
    /* CF(stackn, push, &current); */
  check_next:;
    free(current);
  }
  return num;
}
void prune_states4(ssvect* stackp, ssvect* stackn){
  long long int average_gr = 0; 
  long long int average_g = 0;
  state* current;
  long double num = stackp->size/stackp->element_size / 1024;
  if (!num){
    printf("error empty stack\n");
    return;
  }

  for(int i = 0; i < stackp->size; i+= stackp->element_size){
    current = *((state**)(stackp->head+i));
    average_gr += current->geo_r / num;
    average_g += current->geo / num;
  }
  average_gr /= 1024;
  average_g /= 1024;
  average_g += average_gr*2;
  /* printf("%d %d\n", average_gr, average_sr); */
  while(current = CF(stackp, pop)){
    current = *(state**)current;
    int egeo = current->geo_r*2 + current->geo;
    //printf("%d vs %d; %d vs %d\n", egeo, average.geo, eobs, average.obs);
    if (egeo < average_g){
      free(current);
      continue;
    }
    //states that produce fewer collection robots at the beginning are less likely to be successful
    CF(stackn, push, &current);
  }
}

void prune_states3(ssvect* stackp, ssvect* stackn){
  long double average_gr = 0;
  long double average_g = 0;
  state* current;
  long double num = stackp->size/stackp->element_size / 1024;
  for(int i = 0; i < stackp->size; i+= stackp->element_size){
    current = *((state**)(stackp->head+i));
    average_gr += current->geo_r / num;
    average_g += current->geo / num;
  }
  average_g /= 1024;
  average_gr /= 1024;
  average_g += average_gr*3;
  /* printf("%d %d\n", average_gr, average_sr); */
  while(current = CF(stackp, pop)){
    current = *(state**)current;
    int egeo = current->geo_r*3 + current->geo;
    //printf("%d vs %d; %d vs %d\n", egeo, average.geo, eobs, average.obs);
    if (egeo < average_g){
      free(current);
      continue;
    }
    //states that produce fewer collection robots at the beginning are less likely to be successful
    CF(stackn, push, &current);
  }
}

void prune_states2(ssvect* stackp, ssvect* stackn){
  long double average_gr = 0;
  long double average_sr = 0;
  state* current;
  long double num = stackp->size/stackp->element_size / 1024;
  if (!num){
    printf("error empty stack\n");
    return;
  }
  printf("num = %Lf\n", num);
  for(int i = 0; i < stackp->size; i+= stackp->element_size){
    current = *((state**)(stackp->head+i));
    average_gr += (long double)current->geo_r/ num;
    average_sr += (long double)current->obs_r/ num;
    /* printf("%Lf %Lf\n", average_gr, average_sr); */
    /* printf("%Lf %Lf\n", (long double)current->obs_r/ num, (long double)current->geo_r/ num); */
  }
  average_gr /= 1024;
  average_sr /= 1024;
  printf("%Lf %Lf\n", average_gr, average_sr);
  while(current = CF(stackp, pop)){
    current = *(state**)current;
    int egeo = current->geo_r;
    int eobs = current->obs_r;
    //printf("%d vs %d; %d vs %d\n", egeo, average.geo, eobs, average.obs);
    if (egeo < average_gr || eobs <= average_sr){
      free(current);
      continue;
    }
    //states that produce fewer collection robots at the beginning are less likely to be successful
    CF(stackn, push, &current);
  }
}

void prune_states(ssvect* stackp, ssvect* stackn){
  long double average_ore_r = 0;
  long double average_clay_r = 0;
  long double average_obs_r = 0;
  state* current;
  long double num = stackp->size/stackp->element_size >> 10;
  for(int i = 0; i < stackp->size; i+= stackp->element_size){
    current = *((state**)(stackp->head+i));
    average_ore_r += current->ore_r / num / 1024;
    average_clay_r += current->clay_r/ num / 1024;
    average_obs_r += current->obs_r/ num / 1024;
  }
  while(current = CF(stackp, pop)){
    current = *(state**)current;
    if (current->geo_r){
      CF(stackn, push, &current);
      continue;
    }
    int eclay = current->clay_r;
    int eore = current->ore_r;
    int eobs = current->obs_r;
    if (eclay < average_clay_r && eore < average_ore_r && eobs < average_obs_r){
      free(current);
      continue;
    }
    //states that produce fewer collection robots at the beginning are less likely to be successful
    CF(stackn, push, &current);
  }
}

inline void swap_stacks(ssvect** a, ssvect** b){
  ssvect* temp = *a;
  *a = *b;
  *b = temp;
}
int solve(int id,int r1,int r2,int r3a,int r3b,int r4a,int r4b){
  max_geo = 0;
  state* initial = malloc(sizeof(state));
  state* initial1 = malloc(sizeof(state));
  initial->ore_r = 1;
  initial->clay_r = 0;
  initial->obs_r = 0;
  initial->geo_r = 0;
  initial->ore = 0;
  initial->clay = 0;
  initial->obs = 0;
  initial->geo = 0;
  initial->cost[0] = r1;
  initial->cost[1] = r2;
  initial->cost[2] = r3a;
  initial->cost[3] = r3b;
  initial->cost[4] = r4a;
  initial->cost[5] = r4b;
  initial->ore_rm = (r3a > r4a) ? r3a : r4a;
  initial->clay_rm = r3b;
  initial->option = 0;
  memset(initial->path, 0, 30);
  initial->time = 0;
  memcpy(initial1, initial, sizeof(state));
  initial1->option = 1;
  ssvect stack[2];
  ssvect* stackp = stack;
  ssvect* stackn = stack+1;
  ssvect_init(stackp, 100*sizeof(state*), sizeof(state*));
  ssvect_init(stackn, 100*sizeof(state*), sizeof(state*));
  CF(stackp, push, &initial);
  CF(stackp, push, &initial1);
  long unsigned int stack_size = 0;
  int depth = 0;
  /* printf("1 %p\n", &initial); */
  /* while (depth++ < 12){ */
  /*   printf("depth = %d\n", depth); */
  /*   if(!dev_states(stackp, stackn)) goto done; */
  /*   swap_stacks(&stackp, &stackn); */
  /* } */
  /* stack_size = stackp->size/stackp->element_size; */
  /* prune_states(stackp, stackn); */
  /* swap_stacks(&stackp, &stackn); */
  /* printf("pruned stack %ld\n", stackp->size/stackp->element_size); */
  /* while (depth++ < 21){ */
  /*   printf("depth = %d\n",depth);       */
  /*   if(!dev_states(stackp, stackn)) goto done; */
  /*   swap_stacks(&stackp, &stackn); */
  /* } */
  /* stack_size = stackp->size/stackp->element_size; */
  /* prune_states2(stackp, stackn); */
  /* swap_stacks(&stackp, &stackn); */
  /* printf("pruned stack %ld\n", stackp->size/stackp->element_size); */
  while (depth++ < 26){
    printf("depth = %d\n", depth);      
    if(!dev_states(stackp, stackn)) goto done;
    swap_stacks(&stackp, &stackn);
  }
  depth--;
  stack_size = stackp->size/stackp->element_size;
  prune_states2(stackp, stackn);
  swap_stacks(&stackp, &stackn);
  printf("pruned stack %ld\n", stack_size - stackp->size/stackp->element_size);
  while (depth++ < 30){
    printf("depth = %d\n", depth);      
    if(!dev_states(stackp, stackn)) goto done;
    swap_stacks(&stackp, &stackn);
  }  
  stack_size = stackp->size/stackp->element_size;
  prune_states3(stackp, stackn);
  swap_stacks(&stackp, &stackn);
  printf("pruned stack %ld\n", stack_size - stackp->size/stackp->element_size);
  depth--;
  /* while (depth++ < 31){ */
  /*   printf("depth = %d\n", depth);       */
  /*   if(!dev_states(stackp, stackn)) goto done; */
  /*   swap_stacks(&stackp, &stackn); */
  /* }   */
  /* stack_size = stackp->size/stackp->element_size; */
  /* prune_states4(stackp, stackn); */
  /* swap_stacks(&stackp, &stackn); */
  /* printf("pruned stack %ld\n", stack_size - stackp->size/stackp->element_size); */

  
  while (depth++ < 36){
    printf("depth = %d\n", depth);
    if(!dev_states(stackp, stackn)) goto done;
    swap_stacks(&stackp, &stackn);
  }  
  //prune again
  /* int max_g = 0; */
  /* state* current; */
  /* while(current = CF(stackp, pop)){ */
  /*   current = *(state**)current; */
  /*   if (current->geo > max_g) max_g = current->geo; */
  /* } */
 done:
  free(stackp->head);
  free(stackn->head);
  printf("max geo found for blueprint %d was %d\n", id, max_geo);
  return max_geo;
}




void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  int id, r1, r2, r3a, r3b, r4a, r4b;
  int x;
  int t = 0;
  long long int product = 1;
  time_limit = 32;
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    sscanf(buf, "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.", &id, &r1, &r2, &r3a, &r3b, &r4a, &r4b);
    /* printf("Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.\n", id, r1, r2, r3a, r3b, r4a, r4b); */
    /* printf("%ld\n", strlen(buf)); */
    x = solve(id, r1, r2, r3a, r3b, r4a, r4b);
    printf("blueprint %d max output is %d\n", id, x);
    product *= x;
    if (++t > 2) break;
  }
  printf("ans = %lld\n", product);
  fclose(fp);
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  astates = malloc(50000*sizeof(void*));
  nstates = 0;
  process_fun(input_map, filesize);
  process_fgets();
  munmap(input_map, filesize);
  return 0;
}
