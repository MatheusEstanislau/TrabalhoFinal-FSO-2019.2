#include <stdio.h>
#include <pthread.h>
#include <stdint.h>

// Queremos contar até 1 bilhão 2 vezes!
// Bom exemplo, pois mostra que não é só criar a thread que vai ser mais rápido
// Foi necessário usar o Mutex pois como duas threads estavam rodando o mesmo código, e ele contem o counter++
// Esse counter++ não é atômico, e ocorria que das 

#define BIG 1000000
int counter = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *count_to_big(void *arg) {
  for(int i = 0; i < BIG; i++) {
    pthread_mutex_lock(&lock); // Trava todas as outras threads e deixa somente uma acessar a região crítica
    counter++; // Região Crítica!
    pthread_mutex_unlock(&lock); // Destrava as threads
  }

  return NULL;
}

int main() {
  pthread_t thread;
  
  pthread_create(&thread, NULL, count_to_big, NULL); // Thread nova chamando a count_to_big
  count_to_big(NULL); // main thread chamando a count_to_big

  pthread_join(thread, NULL);

  printf("Done. Counter = %u\n", counter);
  return 0;
}