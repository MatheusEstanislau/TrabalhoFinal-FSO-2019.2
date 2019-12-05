#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *myturn(void *arg) { // Tudo void pois void pode receber qualquer coisa e retornar qualquer coisa
  
  int *value_ptr = (int *) arg;

  for(int i = 0; i < 8; i++) {
    sleep(1);
    printf("My Turn! i: %d | value: %d\n", i, *value_ptr);
    (*value_ptr)++;
  }
  return NULL;
}

void yourturn() {
  for(int i = 0; i < 3; i++) {
    sleep(2);
    printf("Your Turn! %d\n", i);
  }
}


int main() {
  pthread_t thread;

  int value = 0;

  pthread_create(&thread, NULL, myturn, &value);
  yourturn();
  
  pthread_join(thread, NULL);
  printf("Thread terminou! value: %d\n", value);
  return 0;
}