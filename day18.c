#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "simple.h"

uint64_t xyz_to_int(char x,char y,char z){
  uint64_t ret = (unsigned char)x;
  ret = ret << 8;
  ret |= (unsigned char)y;
  ret = ret << 8;
  ret |= (unsigned char)z;
  /* printf("%d, %d, %d, --> %lx\n", x,y,z,ret); */
  return ret;
}

int rfind(ssvect* current, ssvect* next, uint64_t *space, uint64_t *map){
  uint64_t* intp;
  int keep_going = 0;
  while(intp = CF(current, pop)){
    uint64_t point;
    point = *intp;
    char x = (point & 0xFF0000) >> 16;
    char y = (point & 0xFF00) >> 8;
    char z = (point & 0xFF);
    if (x < 0 || y < 0 || z < 0 || z > 40 || y > 40 || x > 40){
      return -1;
    }
    uint64_t dest;
    dest = xyz_to_int(x,y,z-1);
    if (!space[dest]){
      if (!map[dest]){
	map[dest] = 1;
	CF(next, push, &dest);
	keep_going++;
      }
    }
    dest = xyz_to_int(x,y,z+1);
    if (!space[dest]){
      if (!map[dest]){
	map[dest] = 1;
	CF(next, push, &dest);
	keep_going++;
      }
    }
    dest = xyz_to_int(x,y-1,z);
    if (!space[dest]){
      if (!map[dest]){
	map[dest] = 1;
	CF(next, push, &dest);
	keep_going++;
      }
    }
    dest = xyz_to_int(x,y+1,z);
    if (!space[dest]){
      if (!map[dest]){
	map[dest] = 1;
	CF(next, push, &dest);
	keep_going++;
      }
    }
    dest = xyz_to_int(x-1,y,z);
    if (!space[dest]){
      if (!map[dest]){
	map[dest] = 1;
	CF(next, push, &dest);
	keep_going++;
      }
    }
    dest = xyz_to_int(x+1,y,z);
    if (!space[dest]){
      if (!map[dest]){
	map[dest] = 1;
	CF(next, push, &dest);
	keep_going++;
      }
    }
  }
  return keep_going;
}

int dfs(char x, char y, char z, uint64_t *space){
  ssvect stack[2];
  ssvect* stackp = stack;
  ssvect* stackn = stack+1;
  uint64_t point = xyz_to_int(x,y,z);
  uint64_t* visited = malloc(0x1000000*sizeof(uint64_t));
  memset(visited, 0, 0x1000000*sizeof(uint64_t));
  visited[xyz_to_int(x,y,z)] = 1;
  ssvect_init(stackp, 100*sizeof(uint64_t), sizeof(uint64_t));
  ssvect_init(stackn, 100*sizeof(uint64_t), sizeof(uint64_t));
  CF(stackp, push, &point);
  int t;
 loopback:
  t = rfind(stackp, stackn, space, visited);
  if (t > 0){
    ssvect* temp = stackp;
    stackp = stackn;
    stackn = temp;
    goto loopback;
  }  
  free(visited);
  free(stack[0].head);
  free(stack[1].head);
  if (t < 0) return 1;
  return 0;
}

int check_neighbors(uint64_t point, uint64_t* arr){
  char x = (point & 0xFF0000) >> 16;
  char y = (point & 0xFF00) >> 8;
  char z = (point & 0xFF);
  int ret = 0;
  ret += arr[xyz_to_int(x,y,z+1)];
  ret += arr[xyz_to_int(x,y,z-1)];
  ret += arr[xyz_to_int(x,y-1,z)];
  /* ret += arr[xyz_to_int(x,y-1,z+1)]; */
  /* ret += arr[xyz_to_int(x,y-1,z-1)]; */
  ret += arr[xyz_to_int(x,y+1,z)];
  /* ret += arr[xyz_to_int(x,y+1,z-1)]; */
  /* ret += arr[xyz_to_int(x,y+1,z+1)]; */
  ret += arr[xyz_to_int(x-1,y,z)];
  /* ret += arr[xyz_to_int(x-1,y,z+1)]; */
  /* ret += arr[xyz_to_int(x-1,y,z-1)]; */
  /* ret += arr[xyz_to_int(x-1,y-1,z)]; */
  /* ret += arr[xyz_to_int(x-1,y-1,z-1)]; */
  /* ret += arr[xyz_to_int(x-1,y-1,z+1)]; */
  /* ret += arr[xyz_to_int(x-1,y+1,z)]; */
  /* ret += arr[xyz_to_int(x-1,y+1,z-1)]; */
  /* ret += arr[xyz_to_int(x-1,y+1,z+1)]; */
  ret += arr[xyz_to_int(x+1,y,z)];
  /* ret += arr[xyz_to_int(x+1,y,z-1)]; */
  /* ret += arr[xyz_to_int(x+1,y,z+1)]; */
  /* ret += arr[xyz_to_int(x+1,y-1,z)]; */
  /* ret += arr[xyz_to_int(x+1,y-1,z-1)]; */
  /* ret += arr[xyz_to_int(x+1,y-1,z+1)]; */
  /* ret += arr[xyz_to_int(x+1,y+1,z)]; */
  /* ret += arr[xyz_to_int(x+1,y+1,z-1)]; */
  /* ret += arr[xyz_to_int(x+1,y+1,z+1)]; */
  /* printf("ret is %x\n", ret); */
  return ret;
}
void process_fgets(){
  puts("here");
  FILE* fp = fopen("input", "rw");
  char buf[1000];
  int x,y,z;
  uint64_t *space = malloc(0x1000000*sizeof(uint64_t));
  memset(space, 0, 0x1000000*sizeof(uint64_t));
  int count = 0;
  uint64_t point;
  while(fgets(buf, 1000, fp)){
    /* *(buf + strlen(buf) -1) = '\0';//remove '\n' character */
    /* printf("%ld\n", strlen(buf)); */
    sscanf(buf, "%d,%d,%d", &x,&y,&z);
    /* printf("%d,%d,%d\n", x,y,z); */
    count += 6;
    point = xyz_to_int(x,y,z);
    count -= 2*(check_neighbors(point, space));
    space[point] = 1;
    /* printf("surface = %d\n", count); */
  }
  /* for(uint64_t i = 0; i < 0x1000000; ++i){ */
  /*   if (space[i]) continue; */
  /*   count -= check_neighbors(i, space); */
  /* } */
  printf("part1 = %d\n", count);
  for(int i = -1; i < 40; ++i){
    for(int j = -1; j < 40; ++j){
      for(int k = -1; k < 40; ++k){
	uint64_t point = xyz_to_int(i,j,k);
	if (!space[point]){
	  if (!dfs(i,j,k,space)){
	    /* printf("%d,%d,%d is enclosed\n", i,j,k); */
	    count -= check_neighbors(point, space);
	  }
	}
      }
    }
  }
  printf("part2 = %d\n", count);
  
  fclose(fp);
}

int main(int argc, char** argv){
  process_fgets();
  return 0;
}
