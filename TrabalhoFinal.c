#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// Exemplo original em https://en.wikibooks.org/wiki/C_Programming/POSIX_Reference/dirent.h

/*
Fluxo de execução que deve ser seguido
1. Copia o diretório
2. Compacta em bzip2 todos os arquivos da cópia
*/

int number_of_files(DIR *dp){
    struct dirent *en;
    int count = 0;
    while((en = readdir(dp))){
        if(en->d_name != NULL){
            if(strcmp(en->d_name, ".") == 0 || strcmp(en->d_name, "..") == 0){
                continue;
            }
            // puts(en->d_name);
            count++;
        }
    }
    return count;
}

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

void only_zip(char *file_name, const char *dir_path){
    char file_path[500] = "";
    strcat(file_path, dir_path);
    // strcat(file_path, "/");
    strcat(file_path, file_name);

    // if(isdirectory(file_path) != 0){
    //     puts(file_path);
    //     printf("É um diretório\n");

    //     struct dirent *entry;   
    //     DIR *dp;

    //     dp = opendir(file_path);
    
    //     if (dp == NULL){
    //         perror("opendir");
    //         exit(2);
    //     }

    //     while((entry = readdir(dp)) != NULL){
    //         if(strcmp(entry->d_name, ".") == 0){
    //             continue;
    //         }
    //         else if(strcmp(entry->d_name, "..") == 0){
    //             continue;
    //         }
    //         puts(entry->d_name);
    //         only_zip(entry->d_name, file_path);
    //     }
    //     closedir(dp);
    // }

    char bz[500] = "";
    strcat(bz, "bzip2");
    strcat(bz, " ");
    strcat(bz, file_path);
    system(bz);    
}

void copy_dir(const char *dir_path, const char *target_dir){
    char copy[500] = "";
    strcat(copy, "cp -ax ");
    strcat(copy, dir_path);
    strcat(copy, " ");
    strcat(copy, target_dir);
    system(copy);
}

void copy_and_zip(char *file_name, const char *dir_path, const char *target_dir){
    char file_path[500] = "";
    strcat(file_path, dir_path);
    strcat(file_path, file_name);

    // else{
    // chamada para copiar o arquivo para target_dir
    char copy[500] = "";
    strcat(copy, "cp ");
    strcat(copy, file_path);
    strcat(copy, " ");
    strcat(copy, target_dir);
    system(copy);

    // file_path agora vai guardar o caminho da copia
    strcpy(file_path, target_dir);
    strcat(file_path, file_name);

    // chamada para compactar a copia
    char bz[500] = "";
    strcat(bz, "bzip2");
    strcat(bz, " ");
    strcat(bz, file_path);
    system(bz);
    // }
}

void keep_reading(const char *path){
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
                // printf("EH UM DIRETORIO -> ");
                puts(full_path);
                // printf("Chama recursivo (seja o que Deus quiser...)\n");
                keep_reading(full_path);
            }
            else{ // Não é um diretorio, então compacta
                only_zip_full_path(full_path);
            }

            strcpy(full_path, path);
        }
    }

    closedir(dp);
} 

int compact_dir(const char *path, const char *target){
    copy_dir(path, target);
    keep_reading(target);
    return 0;
}

int main(int argc, char **argv) {
    if(argc != 3 || argv[1] == NULL || argv[2] == NULL){
        printf("São esperados 2 argumentos: diretório alvo e diretório destino.\nOperação abortada.\n");
        exit(1);
    }
    
    compact_dir(argv[1], argv[2]);

    return 0;
}

/*

Como seria a função que compacta recursivamente?

funcao(diretorio){
    le o proximo item do diretorio
    
    se o item for um ARQUIVO
        compacta ele
    
    se o item for um DIRETORIO
        funcao(item)
}

*/
