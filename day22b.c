#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "split.h"
#include "simple.h"



int load_fun(char* stream, char** arr, int row){  
  int offset = 0;
  while (stream[offset++] != '\n');
  memcpy(arr[row], stream, offset);
  return offset;
}


int rotate(int dir, char input){
  switch(input){
  case 'R':
    return (dir + 1) % 4;
    break;
  case 'L':
    return (dir + 3) % 4;
    break;
  }
}
int check_square_and_move(int *x, int *y, char** arr, int *dir, int num_rows){
  int dx = 0;
  int dy = 0;//upwards wrapping will fail on other inputs
  switch(*dir){
  case 0:
    dx = 1;
    break;
  case 1:
    dy = 1;
    break;
  case 2:
    dx = -1;
    break;
  case 3:
    dy = -1;
    break;
  }
  if (*y + dy < 0){
    if (*x < 100){
      switch(arr[100+*x][0]){ //EE going up
      case '.':
	*y = 100+*x;
	*x = 0;
	*dir = 0;
	return 0;
      case '#':
	return -1;
      }
    }
    else{
      switch(arr[--num_rows][*x-100]){ //FF going up
      case '.':
	*y = num_rows;
	*x = *x-100;
	/* *dir = 3; */
	return 0;
      case '#':
	return -1;
      }
    }
  }
  if (*y + dy == num_rows){//FF going down
    switch(arr[0][*x+100]){
    case '.':
      *y = 0;
      *x = *x + 100;
      /* *dir = 1; */
      return 0;
    case '#':
      return -1;
    }
  }
  if (*y + dy == 99 && *x < 50 && dy == -1){//AA going up
    switch(arr[50+*x][50]){
    case '.':
      *y = 50+*x;
      *x = 50;
      *dir = 0;
      return 0;
    case '#':
      return -1;
    }
  }
  if (*x + dx == 49 && *y < 100 && dx == -1){
    if (*y >= 50){//AA going left
      switch(arr[100][*y - 50]){//
      case '.':
	*x = *y-50;
	*y = 100;
	*dir = 1;
	return 0;
      case '#':
	return -1;
      }
    }
    else{//DD going left of 50
      switch(arr[149-*y][0]){
      case '.':
	*x = 0;
	*y = 149 - *y;
	*dir = 0;
	return 0;
      case '#':
	return -1;
      }
    }
  }
  if (*x + dx < 0){
    if (*y < 150){
      switch(arr[149-*y][50]){//DD going left of 0
      case '.':
	*x = 50;
	*y = 149-*y;
	*dir = 0;
	return 0;
      case '#':
	return -1;
      }
    }
    else{
      switch(arr[0][*y-100]){//EE going left
      case '.':
	*x = *y - 100;
	*y = 0;
	*dir = 1;
	return 0;
      case '#':
	return -1;
      }
    }
  }
  switch(arr[*y+dy][*x+dx]){
  case '.':
    *x += dx;
    *y += dy;
    return 0;
  case '#':
    return -1;
  case ' ':
  case 0:
    if (dy == 1){
      if (*x <100){
	switch(arr[*x+100][49]){//CC going down
	case '.':
	  *y = *x+100;
	  *x = 49;
	  *dir = 2;
	  return 0;
	case '#':
	  return -1;
	}
      }
      else{//BB going down
	switch(arr[*x-50][99]){
	case '.':
	  *y = *x-50;
	  *x = 99;
	  *dir = 2;
	  return 0;
	case '#':
	  return -1;
	}
      }
    }
    else if (dx == 1){
      if (*y < 100){//BB going right
	switch(arr[49][*y+50]){
	case '.':
	  *x = *y+50;
	  *y = 49;
	  *dir = 3;
	  return 0;
	case '#':
	  return -1;
	}
      }
      else if (*y < 150){//GG going right and y >= 100
	switch(arr[149-*y][149]){
	case '.':
	  *y = 149-*y;
	  *x = 149;
	  *dir = 2;
	  return 0;
	case '#':
	  return -1;
	}
      }
      else{//CC going right
	switch(arr[149][*y - 100]){
	case '.':
	  *x = *y-100;
	  *y = 149;
	  *dir = 3;
	  return 0;
	case '#':
	  return -1;
	}
      }
    }
  case '\n':
    if (*y < 50){
      switch(arr[149-*y][99]){//GG going right from x == 150
      case '.':
	*y = 149-*y;
	*x = 99;
	*dir = 2;
	return 0;
      case '#':
	return -1;
      }
    }
    else if (*y < 100){//BB going right
      switch(arr[49][*y+50]){
      case '.':
	*x = *y + 50;
	*y = 49;
	*dir = 3;
	return 0;
      case '#':
	return -1;
      }
    }
    else if (*y < 150){//GG going right and y >= 100
      switch(arr[149-*y][149]){
      case '.':
	*y = 149-*y;
	*x = 149;
	*dir = 2;
	return 0;
      case '#':
	return -1;
      }
    }
    else{//CC going right
      switch(arr[149][*y - 100]){
      case '.':
	*x = *y-100;
	*y = 149;
	*dir = 3;
	return 0;
      case '#':
	return -1;
      }
    }
  }
}
void movement_fun(char* stream, char** arr, int num_rows){
  int x = 0;
  int y = 0;
  int dir = 0;//0 right, 1 down, 2 left, 3 up
  while(arr[0][x++] != '.');
  x -= 1;
  printf("initial pos %d,%d\n", x,y);
  int t = 0;
  int s;
  /* stream++; */
  /* stream++; */
  for(;;){
    /* printf("%c", *stream); */
    switch(*stream){
    case '\n':
    case 0:
      printf("moving %d in dir %d\n", t, dir);
      while (t){
	s = check_square_and_move(&x, &y, arr, &dir, num_rows);
	if (!s){
	  /* printf("found space x = %d; y = %d; dir = %d\n", x ,y ,dir); */
	  --t;
	  continue;
	}
	else t = 0;
	//finish movement
      }
      printf("final x = %d; y = %d; dir = %d\n", x+1,y+1,dir);
      printf("ans = %d\n", (y+1)*1000 + (x+1)*4 + dir);
      return;
      goto done;
    case '0' ... '9':
      t *= 10;
      t += *stream - '0';
      break;
    case 'L':
      printf("moving %d in dir %d\n", t, dir);
      while (t){
	s = check_square_and_move(&x, &y, arr, &dir, num_rows);
	if (!s){
	  /* printf("found space x = %d; y = %d; dir = %d\n", x ,y ,dir); */
	  --t;
	  continue;
	}
	else t = 0;
      }
      dir = rotate(dir, 'L');
      printf("location %d,%d facing %d\n", x,y, dir);
      break;
    case 'R':
      printf("moving %d in dir %d\n", t, dir);
      while (t){
	s = check_square_and_move(&x, &y, arr, &dir, num_rows);
	if (!s){
	  /* printf("found space x = %d; y = %d; dir = %d\n", x ,y ,dir); */
	  --t;
	  continue;
	}
	else t = 0;
      }
      dir = rotate(dir, 'R');
      printf("location %d,%d facing %d\n", x,y, dir);
      break;
    }
    stream++;    
  }
 done:
  
  
}
int process_fun(char* map, size_t size){
  size_t* offset_arr = find_splits_str(map, size, "\n\n");
  char* charptr;
  char buf[2000];
  memset(buf, 0, 2000);
  char** arr = malloc(10000*sizeof(char*));
  for(int i = 0; i < 10000; ++i){
    arr[i] = malloc(10000);
  }
  if (!offset_arr){puts("something wrong"); exit(1);}
  int t = 0;
  charptr = load_next_split(map, offset_arr, size);
  char* start = charptr;
  charptr += load_fun(charptr, arr, t++);
  int num_rows = 0;//use 0 to ignore bottom row
  /* printf("len = %d, %d\n", offset_arr[1], charptr-map); */
  while(charptr-start < offset_arr[1]){
    charptr += load_fun(charptr, arr, t++);
    num_rows++;
  }
  /* for (int i = 0; i < num_rows; ++i){ */
  /*   printf("%s", arr[i]); */
  /* } */
  //map
  /* printf("rows %d\n", num_rows); */
  
  charptr = load_next_split(map, offset_arr, size);
  /* puts(charptr); */
  /* printf("%s\n", arr[99]); */
  movement_fun(charptr, arr, num_rows);
  
  
  //movement
  free_ebuf();
  free(offset_arr);
  return 0;
}

void process_fgets(){
  FILE* fp = fopen("input", "rw");
  char buf[32768];
  while(fgets(buf, 32768, fp)){
    *(buf + strlen(buf) -1) = '\0';//remove '\n' character
    /* printf("%ld\n", strlen(buf)); */
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
  process_fun(input_map, filesize);
  process_fgets();
  munmap(input_map, filesize);
  return 0;
}
