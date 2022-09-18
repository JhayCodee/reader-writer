#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

/*
    Este programa proporciona una posible solución para el problema de los 
    lectores-escritores utilizando mutex y semaphores.
    - prioridad a los lectores 
    - se Ha utilizado 10 lectores y 5 escritores para demostrar la solución.
*/

sem_t wrt;
pthread_mutex_t mutex;
int contador = 1;
int numero_de_lectores = 0;


// funcion que ejecuta el hilo escritor
void *writer(void *n_escritor) { 

    sem_wait(&wrt); // protege una seccion de codigo pra que no sea interrupida por otro thread

    contador = contador * 2; 
    printf("Escritor[%d] modificó el contador a: %d unidades\n",(*((int *)n_escritor)), contador); 

    sem_post(&wrt); // libera la seccion de codigo protegida
}


// funcion que ejecuta el hilo lector
void *reader(void *n_lector) { 

    pthread_mutex_lock(&mutex);  

    numero_de_lectores++; // incrementa el numero de lectores

    // si es el primer lector, bloquea al escritor
    if(numero_de_lectores == 1) { 
        sem_wait(&wrt); 
    }

    pthread_mutex_unlock(&mutex); 
    
    // seccion de lectura
    printf("Lector[%d] leyó el contador con: %d unidades\n",*((int *)n_lector), contador);

    // El lector adquiere el bloqueo antes de modificar num reader
    pthread_mutex_lock(&mutex);

    numero_de_lectores--; // decrementa el numero de lectores
    
    // si es el ultimo lector, desbloquea al escritor
    if(numero_de_lectores == 0) { 
        sem_post(&wrt); 
    }

    // libera el bloqueo
    pthread_mutex_unlock(&mutex);
}


// funcion principal
int main() {   

    pthread_t lectores[10], escritores[5]; // 10 lectores y 5 escritores
    pthread_mutex_init(&mutex, NULL); // inicializa el mutex ( direccion de memoria del mutex, atributos)
    sem_init(&wrt,0,1); // inicializa el semaforo ( direccion de memoria del semaforo, 0, valor inicial)

    int a[10] = {1,2,3,4,5,6,7,8,9,10}; // array para enumerar los lectores y escritores

    // crea los hilos lectores
    for(int i = 0; i < 10; i++) {
        // (pointer del thread, default atributos(null), funcion a ejecutar, argumento de la funcion)
        pthread_create(&lectores[i], NULL, (void *)reader, (void *)&a[i]);
    }

    // crea los hilos escritores
    for(int i = 0; i < 5; i++) {
        pthread_create(&escritores[i], NULL, (void *)writer, (void *)&a[i]);
    }

    // espera a que los hilos terminen
    for(int i = 0; i < 10; i++) {
        // (pointer del thread, direccion de memoria donde se guardara el valor de retorno)
        pthread_join(lectores[i], NULL);
    }

    // espera a que los hilos terminen
    for(int i = 0; i < 5; i++) {
        pthread_join(escritores[i], NULL);
    }

    // destruye el mutex y el semaforo
    pthread_mutex_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}