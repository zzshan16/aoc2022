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
typedef struct mon{
  int next_t;
  int next_f;
  int div;
  int* items;
  long int count;
  int num_items;
  void (*op)();
  int num;
} mon;

void transfer(mon* dest, mon* src, int index){
  /* puts("printing source"); */
  /* for (int i = 0; i < 5; ++i){ */
  /*   printf("%d ", src->items[i]); */
  /* } */
  /* puts(""); */
  int x = (src->items)[index];
  memmove(src->items+index, src->items+index+1, (src->num_items - index -1) * sizeof(int));
  src->num_items--;
  (dest->items)[dest->num_items++] = x;
  /* printf("%d new size, moved to %d\n", src->num_items, dest->items[dest->num_items-1]); */
  /* puts("printing dest"); */
  /* for (int j = 0; j < dest->num_items; ++j){ */
  /*   printf("%d ", dest->items[j]); */
  /* } */
  /* puts("done"); */
}

void addx(int div, int* x){
  *x += div;
}
void mulx(int div, int* x){
  *x *= div;
}
void square(int div, int* x){
  printf("squaring %d\n", *x);
  *x = *x * *x;
}
int test(int x, int div){
  return !(x%div);
}
void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  int monkey = 0;
  int op = 0;
  int next_t = 0;
  int next_f = 0;
  mon mons[10];
  char *cptr;
  int t;
  int x;
  mon *monp;
  int *intarr = malloc(1000*sizeof(int));
  for(int i = 0; i < 10; ++i){
    (mons+i)->num_items = 0;
    (mons+i)->items = intarr + 100*i;
    (mons+i)->count = 0;
  }
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    if (strlen(buf) < 3){
      monkey++;
      op = 0;
      monp->next_t = next_t;
      monp->next_f = next_f;
      continue;
    }
    switch(buf[2]){
    case 'n':
      monp = mons+monkey;
      goto end_s;
    case 'S':
      //printf("%s\n", buf+18);
      cptr = buf+17;
      t = 0;
      x = 0;
      while(*++cptr != 0){
	switch(*cptr){
	case '0' ... '9':
	  t *= 10;
	  t += *cptr - '0';
	  break;
	case ',':
	  monp->items[x++] = t;
	  monp->num_items++;
	  t = 0;
	  break;
	default:
	  break;
	}
      }
      monp->items[x++] = t;
      monp->num_items++;
      t = 0;
      break;
    case 'O':
      //printf("%c\n", buf[23]);
      switch(buf[23]){
      case '+':
	monp->op = addx;
	break;
      case '*':
	monp->op = mulx;
	break;
      }
      //printf("%d\n", atoi(buf+25));
      if (buf[25] == 'o'){
	monp->op = square;
	monp->num = 0;
      }
      else{
	monp->num = atoi(buf+25);
      }
      break;
    case 'T':
      //printf("%d\n", atoi(buf+21));
      monp->div = atoi(buf+21);
      break;
    default:
      if (!memcmp("    If true: ", buf, 12)){
	//printf("%d\n", atoi(buf+29));
	next_t = atoi(buf+29);
      }
      else{
	next_f = atoi(buf+29);
      }
    }
  end_s:
  }
  for(int i = 0; i < 8; ++i){
    for (int j = 0; j < (mons+i)->num_items; ++j){
      printf("%d ", (mons+i)->items[j]);
    }
    puts("");
  }
  int round = 0;
  while(round++ < 20){
    for(int i = 0; i < 8; ++i){
      int j = 0;
      monp = mons+i;
      printf("monkey %d num = %d\n", i, monp->num_items);
      monp->count += monp->num_items;
      while (monp->num_items){
	(monp->op)(monp->num, monp->items);
	/* printf("%d\n", monp->items[0]); */
	monp->items[0] /= 3;
	/* printf("after division by 3 %d\n", monp->items[0]); */
	/* printf("div %d\n", monp->div); */
	if (test(monp->items[0], monp->div)){
	  /* printf("%d divisible by %d\n", monp->items[0], monp->div); */
	  if (mons+monp->next_t == monp){
	    printf("monp->next = %d\n", monp->next_t);
	  }
	  transfer(mons+monp->next_t, monp, 0);
	}
	else{
	  //printf("%d not divisible by %d\n", monp->items[0], monp->div);
	  transfer(mons+monp->next_f, monp, 0);
	}
	/* printf("%d\n", monp->num_items); */
      }
    }
  }
  long int max = 0;
  long int max1 = 0;
  for(int i = 0; i < 8; ++i){
    for (int j = 0; j < (mons+i)->num_items; ++j){
      printf("%d ", (mons+i)->items[j]);
    }
    puts("");
    monp = mons+i;
    if (monp->count > max){
      max1 = max;
      max = monp->count;
    }
    else if (monp->count > max1){
      max1 = monp->count;
    }
  }
  printf(" %ld is \n", max1*max);
  for(int i = 0; i < 8; ++i){
    printf("monkey %d count %ld\n", i, (mons+i)->count);
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
  //process_fun(input_map, filesize);
  process_fgets();
  munmap(input_map, filesize);
  return 0;
}
