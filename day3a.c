#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"

void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  int t = 0;
  char arr[256];
  memset(arr, 0, 256);
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';
    int length = strlen(buf) /2;
    for(int i = 0; i < length; ++i){
      arr[*(buf+i)] = 1;
    }
    for(int i = length; i <length*2; ++i){
      if (arr[*(buf+i)]){
	if (buf[i] >='a' && buf[i] <='z'){
	  t += buf[i] - 'a' + 1;
	  goto next;
	}
	else{
	  t += buf[i] - 'A' + 27;
	  goto next;
	}
      }
    }
  next:;
    memset(arr, 0, 256);
  }
  printf("t = %d\n", t);
  fclose(fp);
}

int main(int argc, char** argv){
  process_fgets();
  return 0;
}
