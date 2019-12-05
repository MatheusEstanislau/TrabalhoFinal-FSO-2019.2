#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void *myturn(void *arg) {
  
  int *int_ptr = (int *) malloc(sizeof(int));
  *int_ptr = 2;

  for(int i = 0; i < 8; i++) {
    sleep(1);
    printf("My Turn! i: %d | value: %d\n", i, *int_ptr);
    (*int_ptr)++;
  }
  return int_ptr;
}

void yourturn() {
  for(int i = 0; i < 3; i++) {
    sleep(2);
    printf("Your Turn! %d\n", i);
  }
}


int main() {
  pthread_t thread;
  int *result; // ponteiro para receber o ponteiro para inteiro da função myturn()

  pthread_create(&thread, NULL, myturn, NULL);
  yourturn();
  
  pthread_join(thread, (void *) &result); // Cast para void* pois é isso que o myturn retorna, mesmo sabendo que isso é um int o compilador nao sabe
  printf("Thread terminou! o retorno foi: %d\n", *result);
  return 0;
}