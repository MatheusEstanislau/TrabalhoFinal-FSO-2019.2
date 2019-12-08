#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>

#define QUEUE_SIZE 500          // tamanho fixo da fila
#define FILE_SIZE  500          // tamanho fixo dos caminhos de arquivos

typedef struct queue{
    char **files;               // array de strings
    int front;                  // posição do começo
    int rear;                   // posição do fim
    int size;                   // tamanho da fila no momento
} queue;

queue *fila;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

int flag = 0;

queue* create_queue();
int is_queue_full(queue *q);
int is_queue_empty(queue *q);
void enqueue(queue *q, char *string);
char* dequeue(queue *q);

int isdirectory(char *path);
void copy_file(char* path_current, char* path_destiny);
int only_zip_full_path(char *file_full_path);
int make_tar(char *target_dir);
void remove_dot_tar_from_string(char* string);
int remove_dir(char *target_dir);
void keep_reading(char *current, char *destiny);

void *read_dir(void *argv);
void *consume_queue();

int main(int argc, char **argv) {
    if(argc != 3 || argv[1] == NULL || argv[2] == NULL){
        printf("São esperados 2 argumentos: diretório alvo e diretório destino.\nOperação abortada.\n");
        exit(1);
    }

    remove_dot_tar_from_string(argv[2]);
    mkdir(argv[2], 0777);

    fila = create_queue();

    pthread_t read_thread;  
    pthread_t compact_thread1;
    pthread_t compact_thread2;
    pthread_t compact_thread3;
    pthread_t compact_thread4;

    pthread_create(&read_thread, NULL, read_dir, (void *)argv);
    pthread_create(&compact_thread1, NULL, consume_queue, NULL);
    pthread_create(&compact_thread2, NULL, consume_queue, NULL);
    pthread_create(&compact_thread3, NULL, consume_queue, NULL);
    pthread_create(&compact_thread4, NULL, consume_queue, NULL);

    pthread_join(read_thread, NULL);
    pthread_join(compact_thread1, NULL);
    pthread_join(compact_thread2, NULL);
    pthread_join(compact_thread3, NULL);
    pthread_join(compact_thread4, NULL);
    
    make_tar(argv[2]);
    remove_dir(argv[2]);
    
    return 0;
}

queue* create_queue(){
    queue *q = (queue *) malloc(sizeof(queue));
    q->files = (char **) malloc(sizeof(char *) * QUEUE_SIZE);
    q->size = 0;
    q->front = 0;
    q->rear = QUEUE_SIZE - 1;
    return q;
}

int is_queue_full(queue *q){ return (q->size == QUEUE_SIZE); }

int is_queue_empty(queue *q){ return (q->size == 0); }

void enqueue(queue *q, char *string){
    if(is_queue_full(q)) exit(1);

    q->rear = (q->rear + 1) % QUEUE_SIZE;
    q->files[q->rear] = (char *) malloc((sizeof(string + 1)) * sizeof(char) * FILE_SIZE);
    strcpy(q->files[q->rear], string);
    q->size = (q->size + 1) % QUEUE_SIZE;
}

char* dequeue(queue *q){
    if(is_queue_empty(q)) exit(2);

    char *string = q->files[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->size = (q->size - 1) % QUEUE_SIZE;
    return string;
}   

int isdirectory(char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) return 0;
    else return S_ISDIR(statbuf.st_mode);
}

void copy_file(char* path_current, char* path_destiny) {
    FILE *old_file, *new_file;
    
    old_file = fopen(path_current, "r");
    new_file = fopen(path_destiny, "w");

    int read_count = 0;

    fseek (old_file , 0 , SEEK_END);
    int old_file_size = ftell (old_file);
    rewind (old_file);

    char * file_buffer = (char*) malloc (sizeof(char) * old_file_size);
    
    int count_read = fread (file_buffer, 1 ,old_file_size, old_file);
    fwrite(file_buffer, sizeof(char), old_file_size, new_file);

    fclose(new_file);
    fclose(old_file);
    free(file_buffer);
}

int only_zip_full_path(char *file_full_path){
    char bz[500] = "";
    strcat(bz, "bzip2 \"");
    strcat(bz, file_full_path);
    strcat(bz, "\"");
    return system(bz);
}

int make_tar(char *target_dir){
    char tar[500] = "";

    strcat(tar, "tar cf ");
    strcat(tar, target_dir);
    strcat(tar, ".tar ");
    strcat(tar, target_dir);
    return system(tar);
}

void remove_dot_tar_from_string(char* string){
    char temp[500] = ""; // pode ser insuficiente para arquivos grandes...
    strncat(temp, string, strlen(string)-4);
    strcpy(string, temp);
}

int remove_dir(char *target_dir){
    char rm[500] = "";

    strcat(rm, "rm -rf ");
    strcat(rm, target_dir);
    return system(rm);
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

    while((entry = readdir(dp)) != NULL) {
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
                copy_file(path_current, path_destiny);
                enqueue(fila, path_destiny);
                
                if(flag++ == 1)
                    pthread_cond_broadcast(&condition_var); 
            }

            strcpy(path_current, current);
            strcpy(path_destiny, destiny);
        }
    }

    closedir(dp);
}

void *read_dir(void *argv) {
    char ** paths = (char **) argv;
    keep_reading(paths[1], paths[2]);
    return NULL;
}

void *consume_queue() {
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&condition_var, &mutex);

    while(!is_queue_empty(fila)) {
        char * string = dequeue(fila);

        only_zip_full_path(string);
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}