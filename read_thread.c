#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>

// Experimento para testar o funcionamento da função com UMA thread

#define QUEUE_SIZE 1000          // tamanho fixo da fila

typedef struct queue{
    char *files[QUEUE_SIZE];    // array de strings, cada posição equivale a um caminho de arquivo (ex: home/documentos/teste.txt)
    int front;                  // posição do começo
    int rear;                   // posição do fim
    int size;                   // tamanho da fila no momento
}queue;

queue* create_queue(){
    queue *q = (queue *) malloc(sizeof(queue));
    q->size = 0;
    q->front = 0;
    q->rear = QUEUE_SIZE - 1;
    return q;
}

int is_queue_full(queue *q){
    return (q->size == QUEUE_SIZE);
}

int is_queue_empty(queue *q){
    return (q->size == 0);
}

void enqueue(queue *q, char *string){
    if(is_queue_full(q)) exit(1);

    q->rear = (q->rear + 1) % QUEUE_SIZE;
    q->files[q->rear] = string;
    q->size = q->size + 1;
}

char* dequeue(queue *q){
    if(is_queue_empty(q)) exit(2);

    char *string = q->files[q->front]; 
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->size = q->size - 1;
    return string;
}

void print_queue(queue *q){
    if(q->size == 0) printf("FILA VAZIA\n");

    else{
        printf("\nFILA\n");
        int i = 0;
        while(i < q->size){
            puts(q->files[(q->front)+i]);
            i++;
        }
    }
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
    char temp[500] = ""; // pode ser insuficiente para arquivos grandes...
    strncat(temp, string, strlen(string)-4);
    strcpy(string, temp);
}

void remove_dir(char *target_dir){
    char rm[500] = "";

    strcat(rm, "rm -rf ");
    strcat(rm, target_dir);
    int s = system(rm);
}

queue *q;

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
                copy_file(path_current, path_destiny);
                // printf("Destiny: %s\n", path_destiny);    

                // enfileirar path_destiny
                enqueue(q, path_destiny);
                // puts(dequeue(q));
            }

            print_queue(q);

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

    q = create_queue();

    remove_dot_tar_from_string(argv[2]);
    mkdir(argv[2], 0777);
    keep_reading(argv[1], argv[2]);

    // print_queue(q);
    // puts(dequeue(q));

    // puts(dequeue(fila));
    // puts(dequeue(fila));

    // make_tar(argv[2]);
    // remove_dir(argv[2]);

    return 0;
}
