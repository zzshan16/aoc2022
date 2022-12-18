#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "simple.h"

typedef struct rock{
  uint16_t r0, r1, r2, r3;
  /*
   * The bottom row is r0, the top row is r3.
   */
} rock;

uint16_t mask = 0xFE00;//only consider the leftmost 7 bits.

int popcount(uint16_t x){
  return __builtin_popcountl(x);
}

int check_left(rock* r, uint16_t* ap){
  uint16_t t0, t1, t2, t3;
  t0 = r->r0 << 1u;
  t1 = r->r1 << 1u;
  t2 = r->r2 << 1u;
  t3 = r->r3 << 1u;
  if (popcount(t0) != popcount(r->r0)) return 1;
  if (popcount(t1) != popcount(r->r1)) return 1;
  if (popcount(t2) != popcount(r->r2)) return 1;
  if (popcount(t3) != popcount(r->r3)) return 1;
  if (t0 & ap[0]) return 1;
  if (t1 & ap[1]) return 1;
  if (t2 & ap[2]) return 1;
  if (t3 & ap[3]) return 1;
  r->r0 = t0;
  r->r1 = t1;
  r->r2 = t2;
  r->r3 = t3;
  return 0;
}
int check_right(rock* r, uint16_t* ap){
  uint16_t t0, t1, t2, t3;
  t0 = r->r0 >> 1u & mask;
  t1 = r->r1 >> 1u & mask;
  t2 = r->r2 >> 1u & mask;
  t3 = r->r3 >> 1u & mask;
  if (popcount(t0) != popcount(r->r0)) return 1;
  if (popcount(t1) != popcount(r->r1)) return 1;
  if (popcount(t2) != popcount(r->r2)) return 1;
  if (popcount(t3) != popcount(r->r3)) return 1;
  if (t0 & ap[0]) return 1;
  if (t1 & ap[1]) return 1;
  if (t2 & ap[2]) return 1;
  if (t3 & ap[3]) return 1;
  r->r0 = t0;
  r->r1 = t1;
  r->r2 = t2;
  r->r3 = t3;
  return 0;
}

int check_down(rock* r, uint16_t* ap){
  int x = 0;
  x += r->r0 & ap[0];
  x += r->r1 & ap[1];
  x += r->r2 & ap[2];
  x += r->r3 & ap[3];
  return x;
  /*
   * if check_down() is non-zero, overlap exists.
   */
}
void fill_area(rock* r, uint16_t* ap){
  ap[0] |= r->r0;
  ap[1] |= r->r1;
  ap[2] |= r->r2;
  ap[3] |= r->r3;
}
void spawn_rock(rock* r, int type){
  switch(type){
  case 0:
    r->r0 = 0x3C00;
    r->r1 = 0;
    r->r2 = 0;
    r->r3 = 0;
    break;
  case 1:
    r->r0 = 0x1000;
    r->r1 = 0x3800;
    r->r2 = 0x1000;
    r->r3 = 0;
    break;
  case 2:
    r->r0 = 0x3800;
    r->r1 = 0x0800;
    r->r2 = 0x0800;
    r->r3 = 0;
    break;
  case 3:
    r->r0 = 0x2000;
    r->r1 = 0x2000;
    r->r2 = 0x2000;
    r->r3 = 0x2000;
    break;
  case 4:
    r->r0 = 0x3000;
    r->r1 = 0x3000;
    r->r2 = 0;
    r->r3 = 0;
    break;    
  }
  return;
}
int process_fun(char* map, size_t size){
  /* last character is a newline, its really size-1 */
  size--;
  long long int count = 0;
  uint16_t *arr = malloc(100000000*sizeof(uint16_t));
  memset(arr, 0, 100000000*sizeof(uint16_t));
  uint16_t *floor = arr;
  *arr = 0xFFFF;
  rock r;
  rock *rp = &r;
  uint16_t *ap;
  char dir;
  char* cptr = map;
  long long int height = 0;
  int repeat = 0;
  long long int startc, endc, startf, endf;
  while(count < 2022){
    spawn_rock(rp, count%5);
    ap = floor + 4;
    for(;;){
      dir = *cptr++;
      if (cptr >= map+ size){
	cptr -= size;
	/* puts("looping input"); */
	printf("count is %lld, looping input, shape is %lld, height is %lld \n", count, count %5, floor - arr + height);
      }
      switch(dir){
      case '>':
	check_right(rp, ap);
	break;
      case '<':
	check_left(rp, ap);
	break;
      case '\n':
	goto next;
      }
      if (check_down(rp, ap-1)){
	break;
      }
      else{
	ap--;
      }
    next:
    }
    fill_area(rp, ap);
    /* printf("dropped rock on floor + %ld\n", ap - floor -1); */
    if (floor[4]){
      /* printf("raising floor by 4\n"); */
      floor = floor+4;
    }
    if (floor[3]){
      /* printf("raising floor by 3\n"); */
      floor = floor+3;
    }
    else if (floor[2]){
      /* printf("raising floor by 2\n"); */
      floor = floor+2;
    }
    else if (floor[1]){
      /* printf("raising floor by 1\n"); */
      floor = floor+1;
    }
    else if (floor[0]){
      /* printf("not raising floor\n"); */
    }
    else{
      puts("this should not occur");
    }
    ++count;
    if (floor - arr > 10091*500){
      printf("count = %lld\n", count);
      memmove(arr, floor - (10091*50), 10091*50);
      floor -= 10091*50;
      height += 10091*50;
    }
  }
  printf("height after 2022 is %lld\n", floor - arr + height);
  while(count < 1000000000000){
    spawn_rock(rp, count%5);
    ap = floor + 4;
    for(;;){
      dir = *cptr++;
      if (cptr >= map+ size){
	cptr -= size;
	/* puts("looping input"); */
	printf("count is %lld, looping input, shape is %lld, height is %lld \n", count, count %5, floor - arr + height);
	if (repeat == 5){
	  startc = count;
	  startf = floor - arr + height;
	}
	if (repeat == 10){
	  endc = count;
	  endf = floor - arr + height;
	  long long int skip = ((1000000000000 - endc) / (endc - startc)) -1;
	  count += skip * (endc - startc);
	  height += skip * (endf - startf);
	  printf("jumping ahead %lld iterations\n", skip * (endc - startc));
	}
	/* for(int i = 40; i > 0; --i){ */
	/*   uint8_t x = arr[floor-arr-i] >> 8u; */
	/*   for (int i = 0; i < 8; ++i){ */
	/*     if (x & (1u << 7 -i)) printf("#"); */
	/*     else printf("."); */
	/*   } */
	/*   puts(""); */
	/* } */
	repeat++;
      }
      switch(dir){
      case '>':
	check_right(rp, ap);
	break;
      case '<':
	check_left(rp, ap);
	break;
      case '\n':
	goto next_b;
      }
      if (check_down(rp, ap-1)){
	break;
      }
      else{
	ap--;
      }
    next_b:
    }
    fill_area(rp, ap);
    /* printf("dropped rock on floor + %ld\n", ap - floor -1); */
    if (floor[4]){
      /* printf("raising floor by 4\n"); */
      floor = floor+4;
    }
    if (floor[3]){
      /* printf("raising floor by 3\n"); */
      floor = floor+3;
    }
    else if (floor[2]){
      /* printf("raising floor by 2\n"); */
      floor = floor+2;
    }
    else if (floor[1]){
      /* printf("raising floor by 1\n"); */
      floor = floor+1;
    }
    else if (floor[0]){
      /* printf("not raising floor\n"); */
    }
    else{
      puts("this should not occur");
    }
    ++count;
    if (floor - arr > 10091*500){
      printf("count = %lld\n", count);
      memmove(arr, floor - (10091*50), 10091*50);
      floor -= 10091*50;
      height += 10091*50;
    }
  }
  printf("height after 1000000000000 is %lld\n", floor - arr + height);
  /* for(int i = 40; i > 0; --i){ */
  /*   /\* printf("%08B\n",   arr[i]>>8u); *\/ */
  /*   uint8_t x = arr[i] >> 8u; */
  /*   for (int i = 0; i < 8; ++i){ */
  /*     if (x & (1u << 7 -i)) printf("#"); */
  /*     else printf("."); */
  /*   } */
  /*   puts(""); */
  /* } */
  
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
