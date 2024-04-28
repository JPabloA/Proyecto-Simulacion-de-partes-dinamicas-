#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include "utilities.h"
#include "sharedMemory.c"

Line *memoria;
int numLineas;
int currentProccesNumber, ProcessTime, ProcessSize;
pthread_t hilo;
ThreadProcess args;
sem_t *sem_memoria, *sem_bitacora;
FILE *bitacora;

void asignarMemoria(int pid, int tamano, enum Algoritmo algoritmo);
void liberarMemoria(int pid);

int main() {
    // Solicitar el tipo de algoritmoexit
    enum Algoritmo algoritmo;

    printf("Seleccione el algoritmo de asignación de memoria:\n");
    printf("1. Best-Fit\n2. First-Fit\n3. Worst-Fit\n");
    
    int opcion;
    scanf("%d", &opcion);
    
    switch (opcion) {
        case 1:
            algoritmo = BestFit;
            break;
        case 2:
            algoritmo = FirstFit;
            break;
        case 3:
            algoritmo = WorstFit;
            break;
        default:
            printf("Opción inválida\n");
            exit(1);
    }

    int shmid1 = getSharedMemorySegment(FILENAME, 's');
    int shmid2 = getSharedMemorySegment(SHARED_INFO, 'a');

    Line* linesArray = (Line*) attachSharedMemorySegment(shmid1);
    SharedInformation* information = (SharedInformation*) attachSharedMemorySegment(shmid2);

    // linesArray[0].pid = 5;
    // linesArray[0].state = Available;
    // information[0].num_lines = 100;

    // !: while to create the process

    srand(time(NULL));
    while (true){

        args.pid = getProccesID(); 
        currentProccesNumber++;          
        args.lines = rand() % 10 + 1;
        args.time = rand() % 41 + 20;

        pthread_create(&hilo, NULL, &searhForMemory, NULL);
        pthread_join(hilo, NULL);

        // asignarMemoria(pid, tamano, algoritmo);
        // sleep(tiempo);
        // liberarMemoria(pid);


        int delay = rand() % 31 + 30;
        sleep(delay);

    }



    detachSharedMemorySegment(linesArray);
    detachSharedMemorySegment(information);
    
    printf("Wrote data succesfully\n");
    
    // // sem_memoria = sem_open("/sem_memoria", 0);
    // // sem_bitacora = sem_open("/sem_bitacora", 0);
    // // bitacora = fopen("bitacora.log", "a");

    // // Free shared memory (The segment is not available JUST for this process anymore)

    // Generar procesos
    // srand(time(NULL));
    // while (1) {
    //     int tamano = rand() % 10 + 1;
    //     int tiempo = rand() % 41 + 20;
    //     int pid = getpid();

    //     asignarMemoria(pid, tamano, algoritmo);
    //     sleep(tiempo);
    //     liberarMemoria(pid);

    //     int delay = rand() % 31 + 30;
    //     sleep(delay);
    // }

    return 0;
}

int getProccesID(){
    return currentProccesNumber;
}

void* searhForMemory(){

    asignarMemoria(args.pid, args.lines, BestFit);
    sleep(args.time);
    liberarMemoria(args.pid);

    pthread_exit(NULL);
}


// void asignarMemoria(int pid, int size, enum Algoritmo algoritmo) {
//     sem_wait(sem_memoria);

//     int asignado = 0;
//     for (int i = 0; i <= numLineas - size; i++) {
//         int hueco = 1;
//         for (int j = 0; j < size; j++) {
//             if (memoria[i + j].state == InUse) {
//                 hueco = 0;
//                 i += j;
//                 break;
//             }
//         }
//         if (hueco) {
//             for (int j = 0; j < size; j++) {
//                 memoria[i + j].state = InUse;
//                 memoria[i + j].pid = pid;
//             }
//             asignado = 1;
//             break;
//         }
//     }

//     // if (asignado) {
//     //     sem_wait(sem_bitacora);
//     //     fprintf(bitacora, "Proceso %d asignado (%d líneas)\n", pid, size);
//     //     sem_post(sem_bitacora);
//     // } else {
//     //     sem_wait(sem_bitacora);
//     //     fprintf(bitacora, "Proceso %d no pudo ser asignado\n", pid);
//     //     sem_post(sem_bitacora);
//     // }

//     // sem_post(sem_memoria);
// }

// void liberarMemoria(int pid) {
//     sem_wait(sem_memoria);

//     // Liberar la memoria asignada al proceso
//     // ...

//     sem_post(sem_memoria);
// }