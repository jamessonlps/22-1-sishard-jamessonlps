#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

sem_t sem_barrier;
pthread_mutex_t mutex;
int num_barrier = 0;

void *thread_barreira(void *arg) {

    pthread_mutex_lock(&mutex);
    printf("Antes da barreira => %d\n", num_barrier);
    pthread_mutex_unlock(&mutex);

    if (num_barrier > 2) {
        sem_post(&sem_barrier);
        sem_post(&sem_barrier);
        sem_post(&sem_barrier);
    } else {
        pthread_mutex_lock(&mutex);
        num_barrier++;
        pthread_mutex_unlock(&mutex);

        sem_wait(&sem_barrier);
    }

    // TODO: implemente a barreira aqui

    printf("Depois da barreira => %d\n", num_barrier);

    return NULL;
}

int main() {
    // TODO: crie 4 threads rodando o código acima
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem_barrier, 0, 0);

    pthread_t pid_list[4];

    for (int i = 0; i < 4; i++) {
        pthread_create(&pid_list[i], NULL, thread_barreira, NULL);
    }

    for (int j = 0; j < 4; j++) {
        pthread_join(pid_list[j], NULL);
    }

    // TODO: espere por elas

    sem_destroy(&sem_barrier);
    pthread_mutex_destroy(&mutex);

    return 0;
}
