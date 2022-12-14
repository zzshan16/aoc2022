#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>
#include "simple.h"
#include "split.h"

int cmpint(int x, int y){
  return (x - y);
}
  
int find_matching(char* ap){
  int counter = 0;
  char *a = ap;
  for(;;){
    switch(*++a){
    case '[':
      counter++;
      break;
    case ']':
      if (!counter){
	return a - ap;
      }
      else counter--;
      break;
    }
  }
  return -1;
}
int cmp_fun(char* ap, int a_len, char* bp, int b_len){
  char buf1[20];
  char buf2[20];
  char* a = ap;
  char* b = bp;
  if (*ap != '['){
    sprintf(buf1,"[%d]", atoi(a));
    a = buf1;
    ap = a;
  }
  if (*bp != '['){
    sprintf(buf2,"[%d]", atoi(b));
    b = buf2;
    bp = b;
  }
  /* if (a_len == 1 && b_len == 1){ */
  /*   if (*a == '[' && *b == '['){ */
  /*     return 0; */
  /*   } */
  /* } */
  //assert(* == '[')
  char y;
  char x;
  int adist = -1;
  int bdist = -1;
  /* int aindex = 0; */
  /* int bindex = 0; */
  int bval = 0;
  int aval = 0;
  int state;
  int t;
 loopback:
  state = 1;
  adist = -1;
  bdist = -1;
  while (state & 1){
    y = *(++b);
    switch(y){
    case ']':
      state &= ~1;
      state |= 4;
      break;
    case ',':
      state |= 2;
      state &= ~1;
      break;
    case '0' ... '9':
      if (!(state & 8)){
	state |= 8;
	bp = b;
      }
      bval *= 10;
      bval += y - '0';
      break;
    case '[':
      bdist = find_matching(b);
      state |= 16;
      state &= ~1;
      break;
    case '\0':
    case '\n':
      /* printf("error, b %ld detected %x\n", b - bp, *b); */
      state &= ~1;
      state |= 4;
      break;
      /* printf("error, b %ld detected %x\n", b - bp, *b); */
      /* return 0; */
      /* break; */
    }
  }
  while (!(state & 1)){
    x = *(++a);
    switch(x){
    case ']':
      state |= 1;
      state |= 32;
      break;
    case ',':
      state |= 128;
      state |= 1;
      break;
    case '0' ... '9':
      if (!(state & 64)){
	state |= 64;
	ap = a;
      }
      aval *= 10;
      aval += y - '0';
      break;
    case '[':
      /* puts("bracket open a"); */
      state |= 256;
      adist = find_matching(a);
      state |= 1;
      break;
    case '\0':
    case '\n':
      /* printf("error, a %ld, detected %X\n", a - ap, *a); */
      state |= 1;
      state |= 32;
      break;

      /* printf("error, a %ld, detected %X\n", a - ap, *a); */
      /* return 0; */
      /* break; */
    }
  }
  /* printf("state = %d; adist = %d, bdist = %d\n", state, adist, bdist); */
  //comparing 2 integers
  if ((state & 72) == 72){// 8+64
    t = 0 - cmpint(atoi(ap), atoi(bp));
    /* printf("comparing t = %d, a = %d, b = %d\n", t, atoi(ap), atoi(bp)); */
    if ((state & 130) == 130){//2 + 128
      /* puts("2 commas"); */
      if (t) return t;
      goto loopback;
    }
    if ((state & 36) == 36){//4 + 32
      /* puts("2 closing brackets"); */
      if (t) return t;
      return 0;
    }
    if (t) return t;
    /* puts("1 comma 1 closing bracket"); */
    if (state & 32) return 1;
    return -1;
    /* if (a[1] == ','){ */
    /* 	a++; */
    /* 	//puts("adding 1 to a"); */
    /* } */
    /* if (b[1] == ',') b++; */
    /* goto loopback; */
  }
  if ((state & 36) == 36){
    if (state & 64){
      return -1;
    }
    if (state & 8){
      return 1;
    }
    return 0;
  }
  if (adist > 0 && bdist > 0){
    /* puts("splitting"); */
    t = cmp_fun(a, adist, b, bdist);
    if (t) return t;
    //printf("adding adist %d bdist %d\n", adist, bdist);
    //keep going if cmp_fun call was inconclusive
    a += adist;
    b += bdist;
    adist = -1;
    bdist = -1;
    if (a[1] == ',' && b[1] == ','){
      a++;
      b++;
      goto loopback;
    }
    else if (a[1] == ','){
      return -1;
    }
    else if (b[1] == ','){
      return 1;
    }
    else if (a[1] == ']' && b[1] == ']'){
      return 0;
    }
    else{
      puts("unexpected input");
    }
  }
  if (adist > 0){
    if (state & 8){
      t = cmp_fun(a, adist, bp, 0);
      if (t){
	return t;
      }
      else{
	a += adist;
	if (a[1] == ',') ++a;
	goto loopback;
      }
    }
    return -1;
  }
  if (bdist > 0){
    if (state & 64){
      t = cmp_fun(ap, 0, b, bdist);
      if (t) return t;
      b += bdist;
      if (b[1] == ',') ++b;
      goto loopback;
    }
    return 1;
  }
  if (state & 4){
    return -1;
  }
  if (state & 32){
    return 1;
  }
  goto loopback;
}
int cmpq(const void *a, const void *b){  
  int t = cmp_fun(*(char**)a, 0, *(char**)b, 0);
  /* printf("comparing %s and %s: result %d\n", *(char**)a, *(char**)b, t); */
  return -t;
}
int process_fun(char* map, size_t size){
  size_t* offset_arr = find_splits_str(map, size, "\n");
  char* charptr;
  if (!offset_arr){puts("something wrong"); exit(1);}
  int t = 0;
  char buf1[2000];
  char buf2[2000];
  int l1;
  int l2;
  /* printf("%ld splits\n", *offset_arr); */
  int index = 0;
  int sum = 0;
  char** packeta = malloc(1000*sizeof(char*));
  memset(packeta, 0, 1000*sizeof(char*));  
  while(charptr = load_next_split(map, offset_arr, size)){//use set_pos(1) to ignore first segment
    if (*charptr == '\n' || offset_arr[get_pos()-1] >= size){
      continue;
    }
    char* line = malloc(512);
    memset(line, 0, 512);
    /* printf("%ld - %ld\n", offset_arr[get_pos()], offset_arr[get_pos()-1]); */
    if (index == 0){
      memcpy(line, charptr, offset_arr[get_pos()]);
    }
    else{
      memcpy(line, charptr, offset_arr[get_pos()] - offset_arr[get_pos()-1]);
    }
    packeta[index] = line;
    /* printf("%s\n", line); */
    ++index;
  }
  /* for(int i = 0; i < index; i += 2){ */
  /*   printf("%d\n", cmp_fun(packeta[i], 0, packeta[i+1],0)); */
  /* } */

  char* dividers = malloc(20);
  memset(dividers, 0, 20);
  sprintf(dividers, "[[2]]\n");
  sprintf(dividers+10, "[[6]]\n");
  packeta[index++] = dividers;  
  packeta[index++] = dividers+10;
  qsort(packeta, index, sizeof(char*), cmpq);
  /* for(int i = 0; i < index; ++i){ */
  /*   printf("%s", packeta[i]); */
  /* } */
  /* printf("index = %d\n", index); */
  int product = 1;
  for(int i = 0; i < index; ++i){
    if (!memcmp("[[2]]", packeta[i], 5)){
      printf(" 2 found at %d\n", i+1);
      product *= i+1;
    }
    if (!memcmp("[[6]]", packeta[i], 5)){
      printf(" 6 found at %d\n", i+1);
      product *= i+1;
    }
  }
  printf("part2 ans = %d\n", product);
  free_ebuf();
  free(offset_arr);
  return 0;
}


void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    //printf("%d\n", strlen(buf));
  }
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
