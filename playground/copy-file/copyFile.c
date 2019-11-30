#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void copy_file(char* dir_to_copy) {
  FILE *old_file, *new_file;
  
  old_file = fopen(dir_to_copy, "r");
  new_file = fopen("file", "w");

  int char_buffer;
  while(1) {
    char_buffer = fgetc(old_file);
    if(!feof(old_file))
      fputc(char_buffer, new_file);
    else break;
  }

  fclose(new_file);
  fclose(old_file);
}

int main(int argc, char **argv) {
  // ./prog filename_src filename_dest

  copy_file(argv[1]);

  return 0;
}