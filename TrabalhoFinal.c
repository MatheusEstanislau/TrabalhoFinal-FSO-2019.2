#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

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

void only_zip_full_path(char *file_full_path){
    char bz[500] = "";
    strcat(bz, "bzip2");
    strcat(bz, " ");
    strcat(bz, file_full_path);
    system(bz);
}

void copy_dir(char *dir_path, char *target_dir){
    char copy[500] = "";
    strcat(target_dir, ".bz2");

    strcat(copy, "cp -ax ");
    strcat(copy, dir_path);
    strcat(copy, " ");
    strcat(copy, target_dir);
    system(copy);
}

void make_tar(char *target_dir){
    char tar[500] = "";

    strcat(tar, "tar cf ");
    strcat(tar, target_dir);
    strcat(tar, ".tar ");
    strcat(tar, target_dir);
    system(tar);
}

void remove_dir(char *target_dir){
    char rm[500] = "";

    strcat(rm, "rm -rf ");
    strcat(rm, target_dir);
    system(rm);
}

void keep_reading(char *path){
    struct dirent *entry;
    DIR *dp;

    dp = opendir(path);
    
    if (dp == NULL){
        perror("opendir");
        exit(2);
    }

    char full_path[500] = "";
    strcpy(full_path, path);

    while((entry = readdir(dp)) != NULL){
        if(strcmp(entry->d_name, ".") == 0){
            // do nothing
        }
        else if(strcmp(entry->d_name, "..") == 0){
            // do nothing
        }
        else{
            puts(entry->d_name);
            strcat(full_path, "/");
            strcat(full_path, entry->d_name);
            puts(full_path);

            if(isdirectory(full_path) != 0){
                puts(full_path);
                keep_reading(full_path);
            }
            else{
                only_zip_full_path(full_path);
            }

            strcpy(full_path, path);
        }
    }

    closedir(dp);
}

int main(int argc, char **argv) {
    if(argc != 3 || argv[1] == NULL || argv[2] == NULL){
        printf("São esperados 2 argumentos: diretório alvo e diretório destino.\nOperação abortada.\n");
        exit(1);
    }

    copy_dir(argv[1], argv[2]);
    keep_reading(argv[2]);
    make_tar(argv[2]);
    remove_dir(argv[2]);

    return 0;
}
