#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define QUEUE_SIZE 500          // tamanho fixo da fila

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
    q->files[q->rear] = string; // pode não funcionar, por isso uso strcpy
    q->size = q->size + 1;
}

char* dequeue(queue *q){
    if(is_queue_empty(q)) exit(2);

    char *string = q->files[q->front]; // usar strcpy
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->size = q->size - 1;
    return string;
}

int main(){

    queue *fila = create_queue();

    enqueue(fila, "alo");
    enqueue(fila, "eae");
    enqueue(fila, "opa");

    puts(dequeue(fila));
    puts(dequeue(fila));
    puts(dequeue(fila));

    return 0;
}