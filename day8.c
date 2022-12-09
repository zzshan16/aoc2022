#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#define NUM_THREADS 16
void process_char(char* map, size_t offset, size_t size, size_t line_len, long long int *pair){
  if (offset % line_len == line_len-1){
    return;
  }

  char h = map[offset];
  long long int a,b,c,d;
  a = b = c = d = 0;
  long long int position = offset;
  char visible = 0;
  size_t line_len1 = line_len -1;
  //printf("%ld is %c\n", offset, h);
  while (--position % line_len != line_len1){
    if (position < 0) break;
    if(map[position] >= h){
      a++;
      break;
    }
    a++;
  }
  if (position % line_len == line_len1){
    //printf("%ld is visible from left\n", offset);
    visible = 1;
  }
  position = offset;
  while (++position % line_len != line_len1){
    if(map[position] >= h){
      b++;
      break;
    }
    b++;
  }
  if (position % line_len == line_len1){
    //printf("%ld is visible from right\n", offset);
    visible = 1;
  }
  position = offset;
  while ((position = position - line_len) >= 0){
    if(map[position] >= h){
      c++;
      break;
    }
    c++;
  }
  if (position < 0){
    //printf("%ld is visible from above\n", offset);
    visible = 1;
  }
  position = offset;
  while((position = position + line_len) <= size){
    if(map[position] >= h){
      d++;
      break;
    }
    d++;	
  }
  if (position > size){
    //printf("%ld is visible from below\n", offset);
    visible = 1;
  }
  long long int product = a*b*c*d;
  /* if (product){ */
  //printf("%d %d %d %d %d = %d\n", th, a, b, c, d, product);
  /* } */
  if (product > *(pair+1)){
    *(pair+1) = product;
  }
  if (visible){
    //printf("%ld is visible\n", offset);
    (*pair)++;
  }
  return;
}
void* process_multi(void** arr){
  char* map = *arr;
  size_t size = **(size_t**)(arr+1);
  size_t line_len = **(size_t**)(arr+2);
  long long int* pair = *(long long int**)(arr+3);
  size_t begin = **(size_t**)(arr+4);
  size_t end = *(*(size_t**)(arr+4) + 1);
  /* printf("thread beginning at %ld, ending at %ld: pair 0 %p, pair 1 %p\n", */
  /* 	 begin, end, pair, pair+1); */
  for(int i = begin; i < end; ++i){
    process_char(map, i, size, line_len, pair);
  }
  
  return 0;
}
int process_fun(char* map, size_t size){
  char* begin = map;
  size_t line_len = 0;
  while (*map++ != '\n');
  line_len = map - begin;
  /* printf("line len = %ld\n", line_len); */
  /* for(int i = 0; i < 3; ++i){ */
  /*   for(int j = 0; j < line_len -1; ++j){ */
  /*     printf("%c ", begin[i*line_len + j]); */
  /*   } */
  /*   puts(""); */
  /* } */
  long long int arr[2*NUM_THREADS];
  memset(arr, 0, 2*NUM_THREADS * sizeof(long long int));
  char* ptrbuf = malloc(NUM_THREADS*5*sizeof(void*) + (NUM_THREADS+1)*sizeof(size_t));
  memset(ptrbuf, 0, NUM_THREADS*5*sizeof(void*) + (NUM_THREADS+1)*sizeof(size_t));
  size_t split_size = size / NUM_THREADS;
  for(int i = 1; i < NUM_THREADS; ++i){
    *(size_t*)(ptrbuf + NUM_THREADS*5*sizeof(void*) + i*sizeof(size_t)) = split_size * (i);
    /* printf("%X\n", *(size_t*)(ptrbuf + NUM_THREADS*5*sizeof(void*) + i*sizeof(size_t))); */
  }
  *(size_t*)(ptrbuf + NUM_THREADS*5*sizeof(void*) + NUM_THREADS*sizeof(size_t)) = size;
  for(int i = 0; i < NUM_THREADS; ++i){
    *(void**)(ptrbuf+i*5*sizeof(void*)) = begin;
    *(void**)(ptrbuf+i*5*sizeof(void*) + 1*sizeof(void*)) = &size;
    *(void**)(ptrbuf+i*5*sizeof(void*) + 2*sizeof(void*)) = &line_len;
    *(void**)(ptrbuf+i*5*sizeof(void*) + 3*sizeof(void*)) = arr+2*i;
    *(void**)(ptrbuf+i*5*sizeof(void*) + 4*sizeof(void*)) = ptrbuf + NUM_THREADS*5*sizeof(void*) + i*sizeof(size_t);
    /* printf("here %X\n", *(size_t*)*(void**)(ptrbuf + i*5*sizeof(void*) + 4*sizeof(void*))); */
  }
  /* printf("size = %X\n", size); */
  pthread_t *threads = malloc(NUM_THREADS*sizeof(pthread_t));
  for(int i = 0; i < NUM_THREADS; ++i){
    pthread_create(threads+i, 0, (void*(*)(void*))process_multi, (void*)(ptrbuf+i*5*sizeof(void*)));
  }
  for(int i = 0; i < NUM_THREADS; ++i){
    pthread_join(*(threads+i), 0);
  }
  for(int i = 1; i < NUM_THREADS; ++i){
    /* printf("part 1: %ld\n", arr[0]); */
    /* printf("part 2: %ld\n", arr[1]); */

    arr[0] += arr[2*i];
    if (arr[2*i + 1] > arr[1]){
      arr[1] = arr[2*i+1];
    }
  }    
  
  printf("part 1: %lld\n", arr[0]);
  printf("part 2: %lld\n", arr[1]);
  return 0;
}

int main(int argc, char** argv){
  struct stat statbuf;
  int fd = open("input", O_RDONLY);
  fstat(fd, &statbuf);
  size_t filesize = statbuf.st_size;
  char* input_map = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd);
  process_fun(input_map, filesize);
  
  munmap(input_map, filesize);
  return 0;
}
