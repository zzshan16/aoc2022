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
long int gmax = 0;
typedef struct valve{
  char name[4];
  struct valve** neighbors;
  int num_dest;
  int flow;
  int dist[16];
} valve;

typedef struct path{
  char route[30];
  char route2[30];
  uint16_t opened;
  long int value;
  valve* pos;
  valve* pos2;
  int time;
  int time2;
} path;


char* checked_arr;
/* char* names; */
int num_names;
int* name_arr;


/* int name_to_int(char* a){ */
/*   int x = 0; */
/*   char* nameptr = names; */
/*   while(memcmp(a, nameptr, 2)){ */
/*     x++; */
/*     nameptr+=2; */
/*   } */
/*   return x; */
/* } */

size_t check_offset(unsigned char* a, unsigned char* b){
  size_t ret = 0;
  char buf[2];
  uint16_t x;
  x = (a[0] << 8) | a[1];
  ret += name_arr[x];
  ret *= num_names;
  x = (a[2] << 8) | a[3];
  ret += name_arr[x];
  ret *= num_names;
  x = (a[4] << 8) | a[5];
  ret += name_arr[x];
  ret *= num_names;
  x = (b[0] << 8) | b[1];
  ret += name_arr[x];
  ret *= num_names;
  x = (b[2] << 8) | b[3];
  ret += name_arr[x];
  ret *= num_names;
  x = (b[4] << 8) | b[5];
  ret += name_arr[x];
  /* ret *= num_names; */
  /* if (strlen(a) + strlen(b) == 8){ */
  /*   ret += name_to_int(b+6); */
  /* } */
  /* else{ */
  /*   ret += num_names-1; */
  /* } */
  return ret;
}


int find_path2(path* x, ssvect* stackp){
  int offset = 0;
  uint16_t a;
  int b;
  valve* next;
  long int max = x->value;
  if (x->route[6] && x->route2[6]){
    /* printf("%s;%s\n", x->route, x->route2); */
    if (checked_arr[check_offset(x->route, x->route2)])
      return max;
  }
  /* if (strlen(x->route) == 3 && strlen(x->route2) == 3){ */
  /*   if (checked_arr[check_offset(x->route, x->route2)] || checked_arr[check_offset(x->route2, x->route)]) */
  /*     return max; */
  /* } */
  /* else if (strlen(x->route) == 4 && strlen(x->route2) == 3){ */
  /*   if (checked_arr[check_offset(x->route2, x->route)]) */
  /*     return max; */
  /* } */
  /* else if (strlen(x->route) == 3 && strlen(x->route2) == 4){ */
  /*   if (checked_arr[check_offset(x->route, x->route2)]) */
  /*     return max; */
  /* } */
  /* if (x->value > 2000){ */
  /*   printf("current routes %s:%s, value %ld, current positions %s:%s\n", x->route, x->route2, x->value, x->pos->name, x->pos2->name); */
  /* } */
  while (offset < 15){//15 locations with a non-zero flow rate
    a = 1u << offset;
    if (x->opened & a){
      offset++;
      continue;
    }
    next = *(valve**)(stackp->head+offset*sizeof(void*));
    if (!next){
      offset++;
      continue;
    }
    /* printf("current route %s, value %ld, examine %s\n", x->route, x->value, next->name); */
    if ((b = x->pos->dist[offset]) < 29 - x->time){
      path alt;
      memcpy(alt.route, x->route, 30);
      memcpy(alt.route2, x->route2, 30);
      alt.route[strlen(alt.route)] = next->name[0];
      alt.route[strlen(alt.route)] = next->name[1];
      alt.opened = x->opened | a;
      alt.value = x ->value + (29 - x->time - b)*next->flow;
      /* printf("new route %s, value %ld, new value %d\n", alt.route, x->value, alt.value); */
      alt.pos = next;
      alt.pos2 = x->pos2;
      alt.time = x->time + b + 1;
      alt.time2 = x->time2;
      int pathnext;
      if ((pathnext = find_path2(&alt, stackp)) > max) max = pathnext;
    }
    if ((b = x->pos2->dist[offset]) < 29 - x->time2){
      path alt;
      memcpy(alt.route2, x->route2, 30);
      memcpy(alt.route, x->route, 30);
      alt.route2[strlen(alt.route2)] = next->name[0];
      alt.route2[strlen(alt.route2)] = next->name[1];
      alt.opened = x->opened | a;
      alt.value = x ->value + (29 - x->time2 - b)*next->flow;
      /* printf("new route %s, value %ld, new value %d\n", alt.route, x->value, alt.value); */
      alt.pos2 = next;
      alt.pos = x->pos;
      alt.time2 = x->time2 + b + 1;
      alt.time = x->time;
      int pathnext;
      if ((pathnext = find_path2(&alt, stackp)) > max) max = pathnext;
    }

    offset++;
  }
  if (x->route[6] && x->route2[6]){
    checked_arr[check_offset(x->route, x->route2)] = 1;
    checked_arr[check_offset(x->route2, x->route)] = 1;
  }

  /* if (strlen(x->route) == 3 && (strlen(x->route2) == 3 || strlen(x->route2) == 4)){ */
  /*   checked_arr[check_offset(x->route, x->route2)] = 1; */
  /* } */
  /* else if (strlen(x->route) == 4 && strlen(x->route2) == 3){ */
  /*   checked_arr[check_offset(x->route2, x->route)] = 1; */
  /* } */
  return max;
}



int find_path(path* x, ssvect* stackp){
  int offset = 0;
  uint16_t a;
  int b;
  valve* next;
  long int max = x->value;
  while (offset < 15){//15 locations with a non-zero flow rate
    a = 1u << offset;
    /* if (!(x->pos->dist)){ */
    /*   offset++; */
    /*   continue; */
    /* } */
    /* printf("current route %s, value %ld\n", x->route, x->value); */
    if (x->opened & a){
      offset++;
      continue;
    }
    //branch here! one takes offset, one doesnt
    next = *(valve**)(stackp->head+offset*sizeof(void*));
    if (!next){
      offset++;
      continue;
    }
    /* printf("current route %s, value %ld, examine %s\n", x->route, x->value, next->name); */
    if ((b = x->pos->dist[offset]) < 29 - x->time){
      path alt;
      memcpy(alt.route, x->route, 30);
      alt.route[strlen(alt.route)] = next->name[0];
      alt.route[strlen(alt.route)] = next->name[1];
      alt.opened = x->opened | a;
      alt.value = x ->value + (29 - x->time - b)*next->flow;
      /* printf("new route %s, value %ld, new value %d\n", alt.route, x->value, alt.value); */
      alt.pos = next;
      alt.time = x->time + b + 1;
      int pathnext;
      if ((pathnext = find_path(&alt, stackp)) > max) max = pathnext;
    }
    offset++;
  }
  return max;
}

int rfind(ssvect* current, ssvect* next, int count, valve** visited){
  valve* stackp;
  valve* np;
  while(stackp = CF(current, pop)){
    stackp = *(valve**)stackp;
    /* printf("checking %s with flow rate %d", stackp->name, stackp->flow); */
    int max = stackp->num_dest;
    /* printf(" with %d neighbors:", max); */
    int t = 0;    
    /* printf("%s %p\n", stackp->name, stackp->neighbors); */
    /* printf("%s %p\n", stackp->name, &(stackp->neighbors)); */
    /* fflush(stdout); */
    while(t < max){
      /* puts("here"); */
      np = stackp->neighbors[t++];
      uint16_t h = (np->name[0] << 8) + np->name[1];
      if (np->name == visited[h]->name){
	/* printf("(%s) ", np->name); */
	continue;
      }
      else if (visited[h]){
	printf("hash collision detected\n");
      }
      else{
	/* printf("%s ", np->name); */
	visited[h] = np;
	CF(next, push, &np);
      }
    }
    /* puts(""); */
  }  
  return count+1;
}
int bfs(valve* start, valve* end){
  if (start == end) return 0;
  int count = 0;
  ssvect stack[2];
  ssvect* stackp = stack;
  ssvect* stackn = stack+1;
  valve** visited = malloc(0x10000*sizeof(valve*));
  if (!visited){
    puts("malloc failed");
  }
  uint16_t start_h = (start->name[0] << 8) + start->name[1];
  uint16_t dest_h = (end->name[0] << 8) + end->name[1];
  /* printf("bfschecking %s with flow rate %d:", start->name, start->flow); */
  /* printf("bfschecking %s with flow rate %d:", end->name, end->flow); */
  //int num_visited = 0;
  memset(visited, 0, 0x10000*sizeof(valve*));
  visited[start_h] = start;
  ssvect_init(stackp, 100*sizeof(valve*), sizeof(valve*));
  ssvect_init(stackn, 100*sizeof(valve*), sizeof(valve*));
  CF(stackp, push, &start);
  /* printf("%p\n", *(valve**)(stackp->head)); */
  /* return count; */
 loopback:
  count = rfind(stackp, stackn, count, visited);
  if (!(visited[dest_h] == end)){
    ssvect* temp = stackp;
    stackp = stackn;
    stackn = temp;
    if (count < 100){
      goto loopback;
    }
  }
  free(visited);
  free(stackn->head);
  free(stackp->head);
  return count;
}

void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  char bufname[4];
  char buftuns[200];
  int flow;
  int len = 0;
  valve valves[200];
  int count = 0;
  valve* occupied[256*256];
  valve* neighbor;
  memset(occupied, 0, 256*256*sizeof(valve*));
  valve* current;
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    len = strlen(buf);
    sscanf(buf, "Valve %s has flow rate=%d;", bufname,&flow);
    uint16_t offset = (bufname[0] << 8) + bufname[1];
    if (!occupied[offset])
      occupied[offset] = valves+count++;
    current = occupied[offset];
    current->flow = flow;
    (current->name)[0] = bufname[0];
    (current->name)[1] = bufname[1];
    (current->name)[2] = 0;
    int t = 37;
    while(*(buf+(++t)) != 's');
    t += 2;
    int filled = 0;
    while (t < len){
      switch(buf[t]){
      case 'A' ... 'Z':
	buftuns[filled++] = buf[t];
	break;
      }
      t++;
    }
    filled = filled >> 1;
    current->num_dest = filled;
    current->neighbors = malloc(20*sizeof(void*));
    memset(current->dist, 0, 16*sizeof(int));
    for(int i = 0; i < filled; ++i){
      offset = (buftuns[2*i] << 8) + buftuns[2*i + 1];
      if (!occupied[offset])
	occupied[offset] = valves+count++;
      neighbor = occupied[offset];
      current->neighbors[i] = neighbor;      
    }
  }
  /* for(int i = 0; i < count; ++i){ */
  /*   printf("%s has %d neighbors\n", valves[i].name, valves[i].num_dest); */
  /*   for(int j = 0; j < valves[i].num_dest; ++j){ */
  /*     printf("%s ", valves[i].neighbors[j]->name); */
  /*   } */
  /*   puts(""); */
  /* } */
  ssvect stack;
  ssvect* stackp = &stack;
  ssvect_init(stackp, 100*sizeof(valve*), sizeof(valve*));
  
  for(int i = 0; i < count; ++i){
    if (valves[i].flow){
      //printf("%s has %d flow\n", valves[i].name, valves[i].num_dest);
      valve* p = valves+i;
      CF(stackp, push, &p);
    }
  }
  count = stackp->size/stackp->element_size;
  /* names = malloc(2*count + 2); */
  num_names = count + 1;
  /* names[2*count] = 0; */
  /* names[2*count+ 1] = 0; */
  name_arr = malloc(256*256*sizeof(int));
  memset(name_arr, 0, 256*256*sizeof(int));
  for(int i = 0; i < stackp->size; i+= stackp->element_size){
    valve* current = *(valve**)(stackp->head+i);
    /* printf("%s has %d flow\n", current->name, current->flow); */
    int t = i/stackp->element_size;
    /* memcpy(names + 2*t, current->name, 2); */
    uint16_t x = ((unsigned char)(current->name[0]) << 8) | (unsigned char)(current->name[1]);
    name_arr[x] = t + 1;
  }
  //stack has all the valves with a non zero flow
  current = occupied[('A' << 8) + 'A'];
  /* printf("checking names of %s and %s\n", (*(valve**)(stackp->head))->name, (*(valve**)(stackp->head+sizeof(valve*)))->name); */
  /* int depth = bfs(*(valve**)(stackp->head), *(valve**)(stackp->head+sizeof(valve*))); */
  /* printf("%d is dist from %s to %s\n", depth, (*(valve**)(stackp->head))->name, (*(valve**)(stackp->head+sizeof(valve*)))->name); */
  /* printf("%ld\n", stackp->size/stackp->element_size); */
  for(int i = 0; i < stackp->size; i+= stackp->element_size){
    valve* valve_p = *((valve**)(stackp->head+i));
    int t = 0;
    for (int j = 0; j < stackp->size; j += stackp->element_size){
      valve* dest = *((valve**)(stackp->head+j));
      /* printf("checking bfs(%s,%s)\n", valve_p->name, dest->name); */
      valve_p->dist[t++] = bfs(valve_p, dest);
      /* printf("%s is %d distance away from %s\n", (*(valve**)(stackp->head+i))->name, valve_p->dist[t-1], (*(valve**)(stackp->head+j))->name); */
    }
    
  }
  memset(current->dist, 0, 16*sizeof(int));
  for(int i = 0; i < stackp->size; i+= stackp->element_size){
    valve* dest = *((valve**)(stackp->head+i));
    /* printf("checking bfs(%s,%s)\n", current->name, dest->name); */
    current->dist[i/stackp->element_size] = bfs(current, dest);
    /* printf("%s is %d distance away from %s\n", current->name, current->dist[i/stackp->element_size], (*(valve**)(stackp->head+i))->name); */
  }
  checked_arr = malloc(num_names*num_names*num_names*num_names*num_names*num_names);
  if (!checked_arr){
    puts("malloc failed");
  }
  memset(checked_arr, 0, num_names*num_names*num_names*num_names*num_names*num_names);
  path s;
  memset(s.route, 0, 30);
  s.opened = 0;
  s.value = 0;
  s.pos = current;
  s.time = 0;
  int path_len = find_path(&s, stackp);
  printf("part 1: %d\n", path_len);
  memset(s.route, 0, 30);
  memset(s.route2, 0, 30);
  s.opened = 0;
  s.value = 0;
  s.pos = current;
  s.pos2 = current;
  s.time = 4;
  s.time2 = 4;
  path_len = find_path2(&s, stackp);
  printf("part 2: %d\n", path_len);
  /* printf("answer = %d\n", path_len); */
  /* printf("%s has %d neighbors\n", current->name, current->num_dest); */
  /* path begin; */
  /* begin.flow = 0; */
  /* begin.rate = 0; */
  /* begin.loc = current; */
  /* max_path(&begin, 0); */
  /* printf("max is %ld\n", gmax); */
  fclose(fp);
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  process_fun(input_map, filesize);
  process_fgets();
  munmap(input_map, filesize);
  return 0;
}
