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
  int tracker = 0;
  char arr[256];
  memset(arr, 0, 256);
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';
    int length = strlen(buf);
    int x;
    switch(tracker){
    case 0:
      x = 1;
      break;
    case 1:
      x = 2;
      break;
    case 2:
      x = 4;
      break;
    }
    if (x != 4){
      for(int i = 0; i < length; ++i){
	*(arr+*(buf+i)) |= x;
      }
    }
    if (x == 4){
      for (int i = 0; i < length; ++i){
	if (*(arr+*(buf+i)) == 3){
	  if (buf[i] >='a' && buf[i] <='z'){
	    t += buf[i] - 'a' + 1;
	    break;
	  }
	  else{
	    t += buf[i] - 'A' + 27;
	    break;
	  }
	}
      }
      memset(arr, 0, 256);
    }
    tracker = (tracker +1) % 3;
  }  
  printf("t = %d\n", t);
  fclose(fp);
}

int main(int argc, char** argv){
  process_fgets();
  return 0;
}
