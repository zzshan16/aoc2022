#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "simple.h"
int process_fun(char* map, size_t size){
  size_t* offset_arr = find_splits_str(map, size, "\n");
  char* charptr;
  if (!offset_arr){puts("something wrong"); exit(1);}
  int t = 0;
  while(charptr = load_next_split(map, offset_arr, size)){//use set_pos(1) to ignore first segment
    ++t;
    // printf("%ld\n", atol(charptr));
    //printf("line number %d; offset = %lu\n",t, offset_arr[t]);
    //printf("%s", charptr);
  }
  free_ebuf();
  free(offset_arr);
  return 0;
}

size_t monkey_h(char* mon){
  size_t ans = *mon - 'a' + 1;
  ans = ans << 5;
  ans += mon[1] - 'a' + 1;
  ans = ans << 5;
  ans += mon[2] - 'a' + 1;
  ans = ans << 5;
  ans += mon[3] - 'a' + 1;
  return ans;
}

typedef struct monkey{
  int op;
  long long int val;
  size_t arg0;
  size_t arg1;
} monkey;

monkey* humn;
long long int monkey_val(monkey* m, monkey* arr, int t){
  switch(m->op){
  case 1:
    return m->val;    
  case 2:
    return monkey_val(arr + m->arg0, arr,t) - monkey_val(arr+m->arg1,arr,t);
  case 3:
    return monkey_val(arr + m->arg0,arr,t) + monkey_val(arr+m->arg1,arr,t);
  case 4:
    return monkey_val(arr + m->arg0,arr,t) * monkey_val(arr+m->arg1,arr,t);
  case 5:
    return monkey_val(arr + m->arg0,arr,t) / monkey_val(arr+m->arg1,arr,t);
  }
}
long long int monkey_sum(monkey* root, monkey* arr){  
  return monkey_val(root->arg0 + arr, arr,0) - monkey_val(root->arg1 + arr, arr,1);
}

long long int approx_ddx(long long int x, monkey* root, monkey* arr){
  humn->val = x-4;
  long long int a = monkey_sum(root, arr);
  humn->val = x+4;
  long long int b = monkey_sum(root, arr);
  return (b-a) >>3;
}
void monkey_comp(monkey* m, monkey* arr){
  humn = arr + monkey_h("humn");
  long long int left, right;
  long long int value;
  long long int guess = 10000000000ll;
  long long int bottom;
  long long int t = 1;
 loopback:
  humn->val = guess;
  value = monkey_sum(m, arr);
  /* printf("%lld, %lld\n", value, humn->val); */
  if (!value){
    return;
  }
  if (abs(value) < 500){//assume only 1 root within 500
    goto pt2;
  }
  bottom = approx_ddx(guess, m, arr);
  if (bottom != 0){
    guess = guess - (value/bottom);
  }
  else goto pt2;
  goto loopback;
 pt2:
  /* printf("%lld, %lld\n", value, humn->val); */
  if (!value){
    printf("part 2: %lld, %lld\n", humn->val, value);
    return;
  }
  switch(++t & 1){
  case 0:
    humn->val = guess + (t / 2);
    break;
  case 1:
    humn->val = guess - (t / 2);
    break;
  }
  value = monkey_sum(m, arr);
  goto pt2;
}
void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  monkey* monkey_arr = malloc(32*32*32*32*sizeof(monkey));
  memset(monkey_arr, 0, 32*32*32*32*sizeof(monkey));
  monkey* current;
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    current = monkey_arr + monkey_h(buf);
    switch(buf[6]){
    case '0' ... '9':
      current->op = 1;
      current->val = atoi(buf+6);
      break;
    default:
      switch(buf[11]){
      case '-':
	current->op = 2;
	break;
      case '+':
	current->op = 3;
	break;
      case '*':
	current->op = 4;	
	break;
      case '/':
	current->op = 5;
	break;
      }      
      current->arg0 = monkey_h(buf+6);
      current->arg1 = monkey_h(buf+13);
    }
    /* printf("%ld\n", strlen(buf)); */
  }
  current = monkey_arr + monkey_h("root");
  printf("part1: %lld\n", monkey_val(current, monkey_arr, 0));
  monkey_comp(current, monkey_arr);
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
