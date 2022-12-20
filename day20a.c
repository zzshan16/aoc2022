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

int locate_ptr(void* target, void** source){
  int i = 0;
  while(target != source[i++]){
  }
  return i-1;
}

void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  int arr[20000];
  int **mapping = malloc(10000*sizeof(void*));
  int count = 0;
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    /* printf("%ld\n", strlen(buf)); */
    sscanf(buf, "%d", arr+count);
    mapping[count] = arr+count;
    count++;    
  }
  for(int i = 0; i < count; ++i){
    int x = arr[i];
    int offset, new_offset;
    int* n;
    if (x < 0){
      x = x% (count -1);
      /* x = x% count; */
      x += count + count - 2;
    }
    offset = locate_ptr(arr+i, mapping);
    new_offset = offset + (x%(count-1));
    printf("moving %d by %d\n", arr[i], x%(count-1));
    new_offset = new_offset % (count-1);
    if (offset == new_offset){
      continue;
    }
    if (!new_offset)
      new_offset = count-1;
    printf("moving from %d to %d\n", offset, new_offset);
    if (new_offset > offset){
      n = mapping[new_offset];
      mapping[new_offset] = mapping[offset];
      memmove(mapping+offset, mapping+offset+1, (new_offset - offset) * sizeof(int*));
      mapping[new_offset-1] = n;
    }
    else{
      n = mapping[new_offset];
      mapping[new_offset] = mapping[offset];
      memmove(mapping+new_offset+1, mapping+new_offset, (offset - new_offset) * sizeof(int*));
      mapping[new_offset+1] = n;
    }
    /* for(int j = 0; j < count; ++j){ */
    /*   printf("%d\n", *mapping[j]); */
    /* } */
    /* printf("\n\n"); */

  }
  int pos0 = -1;
  for(int i = 0; i < count;++i){
    if (!*mapping[i]){
      pos0 = i;
      break;
    }
  }
  if (pos0 < 0){
    puts("error");
  }
  printf("count = %d\n", count);
  printf("%d %d %d\n", *mapping[(pos0+1000) % count], *mapping[(pos0+2000) % count], *mapping[(pos0+3000) % count]);
  printf("%d\n", *mapping[(pos0+1000) % count] + *mapping[(pos0+2000) % count] + *mapping[(pos0+3000) % count]);
  /* for(int i = 0; i < count; ++i){ */
  /*   printf("%d\n", *mapping[i]); */
  /* } */
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
