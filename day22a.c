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
int check_square_and_move(int *x, int *y, char** arr, int dir, int num_rows){
  int dx = 0;
  int dy = 0;//upwards wrapping will fail on other inputs
  switch(dir){
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
  up_one:
    switch(arr[--num_rows][*x]){
    case '.':
      *y = num_rows;
      return 0;
    case '#':
      return -1;
      break;
    case 0:
    case ' ':
      goto up_one;
    default:
      puts("error wrapping around to the bottom");
      return 4;
    }
    //
  }
  if (*y + dy == num_rows){
    num_rows = 0;
  down_one:
    switch(arr[num_rows][*x]){
    case '.':
      *y = num_rows;
      return 0;
    case '#':
      return -1;
      break;
    case 0:
    case ' ':
      num_rows++;
      goto down_one;
    default:
      puts("error wrapping around to the top");
      return 4;
    }
    //
  }
  if (*x + dx < 0){
    int t = 0;
    while (arr[*y][++t] != '\n');
    t -= 1;
    switch(arr[*y][t]){
    case '#':
      return -1;
    case '.':
      *x = t;
      return 0;
    default:
      puts("error wraping around to the right");
      return 3;
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
   if (dy == -1){
    up_one_a:      
      switch(arr[--num_rows][*x]){
      case '.':
	*y = num_rows;
	return 0;
      case '#':
	return -1;
	break;
      case 0:
      case ' ':
	goto up_one_a;
      default:
	puts("error wrapping around to the bottom");
	return 4;
      }
    }
    else if (dy == 1){
      num_rows = 0;
    down_one_a:
      switch(arr[num_rows][*x]){
      case '.':
	*y = num_rows;
	return 0;
      case '#':
	return -1;
	break;
      case 0:
      case ' ':
	++num_rows;
	goto down_one_a;
      default:
	puts("error wrapping around to the bottom");
	return 4;
      }
    }
    else if (dx == -1){
      int t = 0;
      while (arr[*y][++t] != '\n');
      t -= 1;
      switch(arr[*y][t]){
      case '#':
	return -1;
      case '.':
	*x = t;
	return 0;
      default:
	puts("error wraping around to the right");
	return 3;
      }      
    }
    else if (dx == 1){
      int t = -1;
      while (arr[*y][++t] == ' ');
      switch(arr[*y][t]){
      case '#':
	return -1;
      case '.':
	*x = t;
	return 0;
      default:
	puts("error wraping around to the right");
	return 3;
      }      
    }
    
    puts("unexpected error");
    return 5;
  case '\n':{
    num_rows = 0;
    to_left:
    switch(arr[*y][num_rows]){
    case '.':
      *x = num_rows;
      return 0;
    case '#':
      return -1;
      break;
    case ' ':
      num_rows++;
      goto to_left;
    default:
      puts("error wrapping around to the left");
      return 4;
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
      /* printf("moving %d in dir %d\n", t, dir); */
      while (t){
	s = check_square_and_move(&x, &y, arr, dir, num_rows);
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
	s = check_square_and_move(&x, &y, arr, dir, num_rows);
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
      /* printf("moving %d in dir %d\n", t, dir); */
      while (t){
	s = check_square_and_move(&x, &y, arr, dir, num_rows);
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
  puts(charptr);
  
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
