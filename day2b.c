#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"

int main(){
  FILE* fp = fopen("input", "r");
  char buffer[32768];
  int x = 0;
  while (fgets(buffer, 32768, fp)){
    *(buffer + strlen(buffer) -1) = '\0';//remove '\n' character
    if (!strcmp(buffer, "A X")){
      x += 3;
      x += 0;
      continue;
    }
    if (!strcmp(buffer, "A Y")){
      x += 1;
      x += 3;
      continue;
    }
    if (!strcmp(buffer, "A Z")){
      x += 2;
      x += 6;
      continue;
    }
    if (!strcmp(buffer, "B X")){
      x += 1;
      x += 0;
    }
    if (!strcmp(buffer, "B Y")){
      x += 2;
      x += 3;
    }
    if (!strcmp(buffer, "B Z")){
      x += 3;
      x += 6;
    }
    if (!strcmp(buffer, "C X")){
      x+= 2;
      x+= 0;
    }
    if (!strcmp(buffer, "C Y")){
      x += 3;
      x += 3;
    }
    if (!strcmp(buffer, "C Z")){
      x += 1;
      x += 6;
    }
  }
  fclose(fp);
  printf(" x = %d\n", x);
  return 0;
}
