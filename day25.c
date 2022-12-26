#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "simple.h"

void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  char numbuf[100];
  numbuf[0] = 0;
  memset(numbuf, 0, 100);
  char* cptr;
  char a;
  int* running_total = malloc(99*sizeof(int));
  memset(running_total, 0, 99*sizeof(int));
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    //printf("%ld\n", strlen(buf));
    memcpy(numbuf+1, buf, strlen(buf) +1);
    cptr = numbuf + strlen(buf) + 1;
    int t = 0;
    /* printf("%s\n", numbuf+1); */
    while(a = *--cptr){
      /* printf("%c", a); */
      switch(a){
      case '0':
	break;
      case '1':
	running_total[t] += 1;
	break;
      case '2':
	running_total[t] += 2;
	break;
      case '-':
	running_total[t] += -1;
	break;
      case '=':
	running_total[t] += -2;
	break;
      }
      t++;
    }
    /* printf("\n"); */
  }
  for(int i = 0; i < 98; ++i){
    int a = running_total[i];
    if (a < -2){
      int b = a/5;
      a -= 5*b;
      running_total[i+1] += b;
      if (a < -2){
	a += 5;
	running_total[i+1] -= 1;
      }
    }
    else if (a > 2){
      int b = a/5;
      a -= b*5;
      running_total[i+1] += b;
      if (a > 2){
	a -= 5;
	running_total[i+1] += 1;
      }
    }
    running_total[i] = a;
  }
  int leading = 0;
  for(int i = 98; i >= 0; --i){
    //printf("i = %d, %d\n", i, running_total[i]);
    switch(running_total[i]){
    case 0:
      if (leading){
	printf("0");
      }
      break;
    case 1:
      printf("1");
      leading = 1;
      break;
    case 2:
      printf("2");
      leading = 1;
      break;
    case -1:
      printf("-");
      leading = 1;
      break;
    case -2:
      printf("=");
      leading = 1;
      break;
    }
  }
  printf("\n");
  fclose(fp);
  free(running_total);
}

int main(int argc, char** argv){
  process_fgets();
  return 0;
}
