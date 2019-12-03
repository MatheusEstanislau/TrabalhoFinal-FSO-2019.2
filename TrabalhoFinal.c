#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// Exemplo original em https://en.wikibooks.org/wiki/C_Programming/POSIX_Reference/dirent.h

// retorna != 0 se for diretório
// retorna == 0 se não for diretório
int isdirectory(char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1){
        return 0;
    }
    else{
        return S_ISDIR(statbuf.st_mode);
    }
}

void copy_file(char* path_current, char* path_destiny) {
  FILE *old_file, *new_file;
  
  old_file = fopen(path_current, "r");
  new_file = fopen(path_destiny, "w");

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

void only_zip_full_path(char *file_full_path){
    char bz[500] = "";
    strcat(bz, "bzip2 \"");
    strcat(bz, file_full_path);
    strcat(bz, "\"");
    int s = system(bz);
}

void make_tar(char *target_dir){
    char tar[500] = "";

    strcat(tar, "tar cf ");
    strcat(tar, target_dir);
    strcat(tar, ".tar ");
    strcat(tar, target_dir);
    int s = system(tar);
}

void remove_dot_tar_from_string(char* string){
    char temp[200] = "";
    strncat(temp, string, strlen(string)-4);
    strcpy(string, temp);
}

void remove_dir(char *target_dir){
    char rm[500] = "";

    strcat(rm, "rm -rf ");
    strcat(rm, target_dir);
    int s = system(rm);
}

void keep_reading(char *current, char *destiny){
    struct dirent *entry;
    DIR *dp;

    dp = opendir(current);

    if (dp == NULL){
        perror("Falha ao abrir diretório");
        exit(2);
    }

    char path_current[500] = "";
    char path_destiny[500] = "";

    strcpy(path_current, current);
    strcpy(path_destiny, destiny);

    while((entry = readdir(dp)) != NULL){
        if((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)){
            strcat(path_current, "/");
            strcat(path_current, entry->d_name);
            strcat(path_destiny, "/");
            strcat(path_destiny, entry->d_name);

            if(isdirectory(path_current) != 0){ // se for diretório
                // criar o mesmo diretório na pasta de output
                
                mkdir(path_destiny, 0777);
                keep_reading(path_current, path_destiny);
            }
            else{ // se não for diretório
                // printf("\nCurrent: %s\n", path_current);
                // printf("Destiny: %s\n", path_destiny);
                
                copy_file(path_current, path_destiny);
                only_zip_full_path(path_destiny);
            }

            strcpy(path_current, current);
            strcpy(path_destiny, destiny);
        }
    }

    closedir(dp);
}

int main(int argc, char **argv) {
    if(argc != 3 || argv[1] == NULL || argv[2] == NULL){
        printf("São esperados 2 argumentos: diretório alvo e diretório destino.\nOperação abortada.\n");
        exit(1);
    }

    remove_dot_tar_from_string(argv[2]);
    mkdir(argv[2], 0777);
    keep_reading(argv[1], argv[2]);
    make_tar(argv[2]);
    remove_dir(argv[2]);

    return 0;
}
