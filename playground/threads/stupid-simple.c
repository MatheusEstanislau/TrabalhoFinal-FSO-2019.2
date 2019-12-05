#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *myturn(void *arg) {
  for(int i = 0; i < 8; i++) {
    sleep(1);
    printf("My Turn! %d\n", i);
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

  // Parâmetros: 
  // 1 - Referencia para a struct pthread 
  // 2 - atributos da pthread
  // 3 - Função que será o ponto de entrada da thread(função deve retornar void* e receber void*)
  // 4 - Algo que queira passar para a função ponto de entrada da thread
  pthread_create(&thread, NULL, myturn, NULL);
  yourturn();
  
  pthread_join(thread, NULL); // Espera até uma thread em específico terminar;
  return 0;
}