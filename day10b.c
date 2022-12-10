#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"

/* int process_fun(char* map, size_t size){ */
/*   size_t* offset_arr = find_splits_str(map, size, "\n"); */
/*   char* charptr; */
/*   if (!offset_arr){puts("something wrong"); exit(1);} */
/*   int t = 0; */
/*   while(charptr = load_next_split(map, offset_arr, size)){//use set_pos(1) to ignore first segment */
/*     ++t; */
/*     // printf("%ld\n", atol(charptr)); */
/*     //printf("line number %d; offset = %lu\n",t, offset_arr[t]); */
/*     //printf("%s", charptr); */
/*   } */
/*   free_ebuf(); */
/*   free(offset_arr); */
/*   return 0; */
/* } */
int check_cycle_count(int cycle_count){
  switch(cycle_count){
  case 20:
  case 60:
  case 100:
  case 140:
  case 180:
  case 220:
    return 1;
  default:
    return 0;
  }
}
void draw(int cycle_count, int x){
  int pos = ((cycle_count -1)% 40);
  switch(x-pos){
  case 1:
  case 0:
  case -1:
    printf("#");
    break;
  default:
    printf(".");
    break;
  }
  if (pos == 39){
    puts("");
  }
}
void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  int x_reg = 1;
  int cycle_count = 0;
  int previous_cycle = 0;
  int p1 = 0;
  int skip = 0;
  while((cycle_count < 250) && fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    previous_cycle = cycle_count;
    char inst[5];
    inst[4] = 0;
    memcpy(inst, buf, 4);
    int add = 0;
    if (!strcmp(inst, "noop")){
      //puts("noop");
      cycle_count++;
      draw(cycle_count, x_reg);
      /* if (check_cycle_count(cycle_count)){ */
      /* 	p1 += x_reg * cycle_count; */
      /* } */
      
    }
    if (!strcmp(inst, "addx")){
      //puts("addx");
      
      cycle_count++;
      /* if (check_cycle_count(cycle_count)){ */
      /* 	p1 += x_reg*cycle_count; */
      /* } */
      draw(cycle_count, x_reg);
      cycle_count++;
      /* if (check_cycle_count(cycle_count)){ */
      /* 	p1 += x_reg*cycle_count; */
      /* } */
      draw(cycle_count, x_reg);
      add += atoi(buf+5);
    }
    /* if (previous_cycle % 20 > cycle_count %20 ){ */
    /*   if (cycle_count %20 == 1){ */
    /* 	puts("%20 = 1"); */
    /* 	p1 += x_reg; */
    /*   } */
    /*   else{	 */
    /* 	printf("%d\n", cycle_count%20); */
    /* 	p1+= x_reg; */
    /*   } */

      /* if (previous_cycle %20 == 19){ */
      /* 	p1+= x_reg; */
      /* } */
      /* else{ */
      /* 	p1 += x_reg + add; */
      /* } */
    /* } */
    /* printf("cycle count %d, x = %d\n", cycle_count, x_reg); */
    x_reg += add;
    
  }
  //printf("p1 %d\n", p1);
  fclose(fp);
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  //process_fun(input_map, filesize);
  process_fgets();  
  munmap(input_map, filesize);
  return 0;
}
