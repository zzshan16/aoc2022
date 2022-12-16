#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "simple.h"
typedef struct sen{
  int x;
  int y;
  int dist;
  int beax;
  int beay;
  int b0, b1, b2, b3;  
} sen;
void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  int num_sens = 0;
  sen sens[100];
  sen* senp;
  char *arr = malloc(10000000);
  int *barr = malloc(10000*sizeof(int));
  memset(barr, 0, 10000*sizeof(int));
  int* barr_0 = barr;
  int* barr_1 = barr + 2500;
  int* barr_2 = barr + 5000;
  int* barr_3 = barr + 7500;
  if (!arr){
    puts("malloc failed");
    exit(4);
  }
  memset(arr, 0, 10000000);
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';
    int sensx;
    int sensy;
    int beax;
    int beay;
    int dist;
    int a = sscanf(buf, "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", &sensx, &sensy, &beax, &beay);
    if (a == 4){
      dist = abs(beax - sensx) + abs(beay-sensy);
      sens[num_sens].x = sensx;
      sens[num_sens].y = sensy;
      sens[num_sens].dist = dist;
      sens[num_sens].beax = beax;
      sens[num_sens].beay = beay;
      senp = sens +num_sens;
      senp->b0 = sensy + sensx + dist;
      senp->b1 = sensy + sensx - dist;
      senp->b2 = sensy - sensx + dist;
      senp->b3 = sensy - sensx - dist;
      barr_0[num_sens] = senp->b0;
      barr_1[num_sens] = senp->b1;
      barr_2[num_sens] = senp->b2;
      barr_3[num_sens] = senp->b3;
      num_sens++;
    }
  }
  printf("num_sens = %d\n", num_sens);
  int count = 0;
  int constants[2];
  for(int i = 0; i < num_sens; ++i){
    int x = barr_0[i];
    int x2 = barr_2[i];
    for(int j = 0; j < num_sens; ++j){
      if (barr_1[j] == x + 2){ printf("found potential stripe at y = -x + %d\n", x + 1);
	constants[0] = x+1; count++;
      }
      if (barr_3[j] == x2 +2){ printf("found potential stripe at y = x + %d\n", x2 + 1);
	constants[1] = x2+1; count++;
      }
    }
  }
  int x = (constants[0] - constants[1]) / 2;
  int y = constants[0] - x;
  
  if (count == 2){
    printf("found unique point at %d,%d\n", x, y);
    printf("ans = %lld\n", x * 4000000ll + y);
  }
  done:
  fclose(fp);
}

int main(int argc, char** argv){
  process_fgets();
  return 0;
}
