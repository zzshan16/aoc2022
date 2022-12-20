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
  int ore_r;
  int clay_r;
  int obs_r;
  int geo_r;
  int ore_rm;
  int ore;
  int clay;
  int obs;
  int geo;
  int cost[6];
  int option;
  char path[30];
  int time;
} state;

state* branch_state(state* x, int option){
  state* new_state = malloc(sizeof(state));
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
  if (current->geo > max_geo){
    max_geo = current->geo;
    return;
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
  /* printf("current stack size %d\n", stackp->size/stackp->element_size); */
  while(current = CF(stackp, pop)){
    ++num;
    /* printf("2 %p\n", current); */
    current = *(state**)current;
    /* printf("3 %p\n", current); */
    /* print_path(current); */
    /* printf("current time is %d, option is %d\n", current->time, current->option); */
    switch(current->option){
    case 0:
      while(current->time < time_limit && current->ore < current->cost[0]){
	update_time(current);
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
      next = branch_state(current, 1);
      CF(stackn, push, &next);
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
      while(current->time < time_limit && current->ore < current->cost[1]){
	update_time(current);
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
      next = branch_state(current, 1);
      CF(stackn, push, &next);
      next = branch_state(current, 2);
      CF(stackn, push, &next);
      if (current->obs_r){
	next = branch_state(current, 3);
	CF(stackn, push, &next);
      }
      
      break;
    case 2:
      while(current->time < time_limit && (current->ore < current->cost[2] || current->clay < current->cost[3])){
	update_time(current);
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
      next = branch_state(current, 1);
      CF(stackn, push, &next);
      next = branch_state(current, 2);
      CF(stackn, push, &next);
      next = branch_state(current, 3);
      CF(stackn, push, &next);
      
      break;
    case 3:
      while(current->time < time_limit && (current->ore < current->cost[4] || current->obs < current->cost[5])){
	update_time(current);
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
      next = branch_state(current, 1);
      CF(stackn, push, &next);
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
void prune_states(ssvect* stackp, ssvect* stackn){
  state average;
  state* current;
  for(int i = 0; i < stackp->size; i+= stackp->element_size){
    current = *((state**)(stackp->head+i));
    average.ore_r += current->ore_r;
    average.clay_r += current->clay_r;
    average.obs_r += current->obs_r;
    /* average.geo_r += current->geo_r; */
    average.ore += current->ore_r;
    average.clay += current->ore_r;
    average.obs += current->ore_r;
    /* average.geo += current->geo; */
  }
  int num = stackp->size/stackp->element_size;
  average.ore_r /= num;
  average.clay_r /= num;
  average.obs_r /= num;
  average.ore /= num;
  average.clay /= num;
  average.obs /= num;
  average.ore += average.ore_r * 7;
  average.clay += average.clay_r * 7;
  average.obs += average.obs_r * 7;
  while(current = CF(stackp, pop)){
    current = *(state**)current;
    if (current->geo_r){
      CF(stackn, push, &current);
      continue;
    }
    int eclay = current->clay + current->clay_r * 7;
    int eore = current->ore + current->ore_r * 7;
    int eobs = current->obs + current->obs_r * 7;
    if (eclay < average.clay && eore < average.ore && eobs < average.obs){
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
  initial->ore_rm = (r3a > r2) ? r3a : r2;
  initial->ore_rm = (initial->ore_rm > r4a) ? initial->ore_rm : r4a;
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
  int depth = 0;
  /* printf("1 %p\n", &initial); */
  while (depth++ < 30){
    if(!dev_states(stackp, stackn)) break;
    swap_stacks(&stackp, &stackn);
  }
  //prune again
  /* int max_g = 0; */
  /* state* current; */
  /* while(current = CF(stackp, pop)){ */
  /*   current = *(state**)current; */
  /*   if (current->geo > max_g) max_g = current->geo; */
  /* } */
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
  long long int sum = 0;
  time_limit = 24;
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    sscanf(buf, "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.", &id, &r1, &r2, &r3a, &r3b, &r4a, &r4b);
    /* printf("Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.\n", id, r1, r2, r3a, r3b, r4a, r4b); */
    /* printf("%ld\n", strlen(buf)); */
    x = solve(id, r1, r2, r3a, r3b, r4a, r4b);
    printf("blueprint %d max output is %d\n", id, x);
    sum += ++t * x;
  }
  printf("ans = %lld\n", sum);
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
