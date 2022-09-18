#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

/*
    this program provides a possible solution for first reader writer problem usiing mutex and semaphores.

*/

sem_t wrt; //a binary semaphore to check both mutual exclusion and race condition
pthread_mutex_t mutex;
int cnt = 1;
int numreader = 0;

void *writer(void *wno) {

    // to ensure resource allocation to writer process
    sem_wait(&wrt);
    //writer section (cs)
    cnt = cnt*2;
    printf("Writer %d modified cnt to %d\n", (*((int *)wno)), cnt);
    //to release resource allocation to writer process
    sem_post(&wrt);
}

void *reader(void *rno) {
    
    // acquire the lock before modifying numreader
    pthread_mutex_lock(&mutex);
    numreader++;
    
    if (numreader == 1) {
        //to ensure shared resource allocation to reader process
        sem_wait(&wrt); // if this id the first reader, then it will block the writer
    }

    pthread_mutex_unlock(&mutex);
    
    // reader section (cs)
    cnt++;
    printf("Reader %d: read cnt as %d\n", *((int *)rno), cnt);

    // reader acquires the lock before modifying numreader
    pthread_mutex_lock(&mutex);
    numreader--;

    if (numreader == 0) {
        //to release resource allocation to reader process
        sem_post(&wrt); // if this is the last reader, it will wake up the writer
    }

    pthread_mutex_unlock(&mutex);
}

int main() {

    pthread_t read[3], write[3];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&wrt, 0, 1);

    int a[3] = {1, 2, 3}; // just used for numbering the writer and reader

    for (int i = 0; i < 3; i++) { 
        pthread_create(&read[i], NULL, (void *)reader, (void *)&a[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_create(&write[i], NULL, (void *)writer, (void *)&a[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(read[i], NULL);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(write[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}
