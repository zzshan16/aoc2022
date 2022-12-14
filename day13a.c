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
    printf("%s\n", buf2);
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
      printf("error, b %ld detected %x\n", b - bp, *b);
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
      printf("error, a %ld, detected %X\n", a - ap, *a);
      state |= 1;
      state |= 32;
      break;

      /* printf("error, a %ld, detected %X\n", a - ap, *a); */
      /* return 0; */
      /* break; */
    }
  }
  printf("state = %d; adist = %d, bdist = %d\n", state, adist, bdist);
  //comparing 2 integers
  if ((state & 72) == 72){// 8+64
    t = 0 - cmpint(atoi(ap), atoi(bp));
    printf("comparing t = %d, a = %d, b = %d\n", t, atoi(ap), atoi(bp));
    if ((state & 130) == 130){//2 + 128
      puts("2 commas");
      if (t) return t;
      goto loopback;
    }
    if ((state & 36) == 36){//4 + 32
      puts("2 closing brackets");
      if (t) return t;
      return 0;
    }
    if (t) return t;
    puts("1 comma 1 closing bracket");
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
    puts("splitting");
    t = cmp_fun(a, adist, b, bdist);
    if (t) return t;
    //printf("adding adist %d bdist %d\n", adist, bdist);
    //keep going if cmp_fun call was inconclusive
    a += adist;
    b += bdist;
    adist = -1;
    bdist = -1;
    if (a[1] == ',' && b[1] == ','){
      puts("incrementing both");
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
int process_fun(char* map, size_t size){
  size_t* offset_arr = find_splits_str(map, size, "\n\n");
  char* charptr;
  if (!offset_arr){puts("something wrong"); exit(1);}
  int t = 0;
  char buf1[2000];
  char buf2[2000];
  int l1;
  int l2;
  printf("%ld splits\n", *offset_arr);
  int index = 0;
  int sum = 0;
  char** packeta = malloc(1000*sizeof(char*));
  memset(packeta, 0, 1000*sizeof(char*));
  
  while(charptr = load_next_split(map, offset_arr, size)){//use set_pos(1) to ignore first segment
    ++index;
    printf("index = %d\n", index);
    int x = 0;    
    while(charptr[++x] != '\n');
    memset(buf1, 0, 2000);
    memset(buf2, 0, 2000);
    memcpy(buf1, charptr, x);
    int y = x+1;
    while(charptr[++y] != '\n');
    memcpy(buf2, charptr+x+1, y-x);
    printf("comparing %s with %s", buf1, buf2);
    t = cmp_fun(charptr, x, charptr + x+1, y-x);//the sizes arent used
    if (t > 0){
      sum += index;
      puts("adding index");
    }
  }
  printf("sum = %d\n", sum);
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
