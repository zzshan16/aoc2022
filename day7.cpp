extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
}

long int target = 8729145l;
char* globals;
size_t global_len;
typedef struct dir{
  char* name;
  struct dir* parent;
  struct dir** children;
  int num_child;
  char** files;
  size_t num_files;
  size_t max_files;
  size_t max;
  long int total_size;
  int filesize[100];
} dir;
long int recursive_size(dir* x);
dir* closest;
long int nearest = 70000000l;
void add_to_sum(long int* sum, dir* parent){
  if (!parent) return;
  // printf("in dir %s\n", parent->name);
  for(int i = 0; i < parent->num_child; ++i){
    add_to_sum(sum,*(parent->children+i));
  }
  if (parent->total_size > 100000){
    // printf("skipped sum of %ld\n", parent->total_size);
    return;
  }
  // long int child_size = recursive_size(parent) - parent->total_size;
  long int this_size = recursive_size(parent);
  if (this_size <= 100000){
    *sum += this_size;
    // printf("new sum is %ld\n", *sum);
    // printf("from parent %ld\n", parent->total_size);
    // printf("from child %ld\n", this_size - parent->total_size);
  }
  else{
    // printf("skipped sum of %ld including children\n", this_size);
  }
}

void add_file(dir* parent, char* buf){
  int x = 0;
  int sum = 0;
  while(x < strlen(buf)){
    switch(buf[x]){
    case '0' ... '9':
      sum *= 10;
      sum += buf[x] - '0';
      break;
    case ' ':
      buf = buf + x +1;
      goto done_getting_int;
    }
    x++;
  }
 done_getting_int://buf is now the name of the file
  for (int i = 0; i < parent->num_files; ++i){
    if (!strcmp(*(parent->files + i), buf)){//found
      parent->total_size -= (parent->filesize)[i];
      (parent->filesize)[i] = sum;
      parent->total_size += sum;
      return;
    }
  }
  parent->num_files++;
  if (parent->num_files > parent->max_files){
    if (char** new_files = (char**)realloc(parent->files, parent->max_files * 2)){
      parent->files = new_files;
      parent->max_files *=2;
    }
    else{
      puts("error realloc");
      exit(2);
    }
  }
  memmove(globals+global_len, buf, strlen(buf));
  *(parent->files + parent->num_files -1) = globals+global_len;
  //next_dir->name = globals+global_len;
  global_len += strlen(buf) + 1;
  (parent->filesize)[parent->num_files -1] = sum;
  parent->total_size += sum;
  // printf("total filesize of parent is %ld\n", parent->total_size);
  return;
}

dir* add_dir(dir* parent, char* dir_name){
  for(int i = 0; i < parent->num_child;++i){
    if (!strcmp(dir_name, ((*(parent->children+i))->name))){
      return *(parent->children+i);
    }
  }
  dir* next_dir = (dir*)malloc(sizeof(dir));
  memset(next_dir->filesize, 0, 100*sizeof(int));
  next_dir->num_files = 0;
  next_dir->max_files = 1000;
  next_dir->files = (char**)malloc(1000*sizeof(char*));
  next_dir->total_size = 0;
  memmove(globals+global_len, dir_name, strlen(dir_name));
  next_dir->name = globals+global_len;
  global_len += strlen(dir_name) + 1;
  next_dir->parent = parent;
  next_dir->children = (dir**)malloc(100*sizeof(dir*));
  next_dir->num_child = 0;
  next_dir->max = 100;
  (parent->num_child)++;
  if (parent->num_child > parent->max){
    if (dir** new_children = (dir**)realloc(parent->children, parent->max * 2)){
      parent->children = new_children;
      parent->max *=2;
    }
    else{
      puts("error realloc");
      exit(2);
    }
  }
  *(parent->children+parent->num_child -1) = next_dir;
  return next_dir;
}
long int recursive_size(dir* x){
  long int tot_size = 0;
  tot_size += x->total_size;
  for(int i = 0; i < x->num_child; ++i){
    tot_size += recursive_size(*(x->children+i));
  }
  return tot_size;
}
void compare_fun(dir* parent){
  if (!parent) return;
  //if (parent->total_size > target) return;
  long int x = recursive_size(parent);
  if (!(x > nearest)){
    if (x < target) return;
    closest = parent;
    nearest = x;
  }
  for(int i = 0; i < parent->num_child; ++i){
    compare_fun(*(parent->children + i));
  }
}

void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  struct dir root;
  memset(root.filesize, 0, 100*sizeof(int));
  root.num_files = 0;
  root.max_files = 1000;
  root.files = (char**)malloc(1000*sizeof(char*));
  root.total_size = 0;
  root.children = (dir**)malloc(100*sizeof(void*));
  root.num_child = 0;
  root.max = 100;
  dir* current_dir = &root;
  memmove(globals+global_len, "root", 4);
  root.name = globals+global_len;
  global_len += 5;

  int mode = 0;
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    switch(*buf){
    case '$'://input is a command
      switch(buf[2]){
      case 'c'://cd
	{
	if (!strcmp("$ cd /", buf)){
	  current_dir = &root;
	  break;
	}
	if (!strcmp("$ cd ..", buf)){
	  current_dir = current_dir->parent;
	  break;
	}
	for(int i = 0; i < current_dir->num_child;++i){
	  // if (!strcmp(buf+5, (*(current_dir->children)+i)->name)){
	  //   current_dir = *(current_dir->children+i);
	  //   goto next;
	  // }
	}
	current_dir = add_dir(current_dir, buf+5);
	// dir* next_dir = (dir*)malloc(sizeof(dir));
	// memmove(globals+global_len, buf+5, strlen(buf+5));
	// next_dir->name = globals+global_len;
	// global_len += strlen(buf+5) + 1;
	// next_dir->parent = current_dir;
	// next_dir->children = (dir**)malloc(100*sizeof(dir*));
	// next_dir->num_child = 0;
	// next_dir->max = 100;
	// (current_dir->num_child)++;
	// if (current_dir->num_child > current_dir->max){
	//   if (dir** new_children = (dir**)realloc(current_dir->children, current_dir->max * 2)){
	//     current_dir->children = new_children;
	//     current_dir->max *=2;
	//   }
	//   else{
	//     puts("error realloc");
	//     exit(2);
	//   }
	// }
	break;
	}
      case 'l'://ls
	mode = 1;
	break;
      default:
	break;
      }
      break;
    case 'd'://input is directory
      {
	char* dir_name = buf+4;
	add_dir(current_dir, dir_name);
      }
      break;
    case '0' ... '9'://is filesize
      {
	add_file(current_dir, buf);
      }
      break;
    default:
      puts("werid input");
      break;
    }
  next:
    ;
  }
  long int recursive_sum = 0;
  current_dir = &root;
  add_to_sum(&recursive_sum, current_dir);
  printf("part1 = %ld\n", recursive_sum);
  compare_fun(&root);
  printf("part2 = %ld\n", nearest);
  fclose(fp);
}

int main(int argc, char** argv){
  globals = (char*)malloc(1000000);
  global_len = 0;
  memset(globals, 0, 1000000);
  process_fgets();
  return 0;
}
