#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include "utilities.h"

int main(int argc, char const *argv[]) {

    key_t key = ftok(FILENAME, 's');

    // Obtener acceso a la memoria compartida y los semáforos
    int shmid = shmget(key, 0, 0);
    if (shmid == -1) {
        perror("Error al obtener la memoria compartida");
        exit(1);
    }
    Line *memoria = (Line*) shmat(shmid, NULL, 0);

    printf("Memoria asignada...\n");
    printf("Valor en celda 0: pid=%d   state=%d\n", memoria[0].pid, memoria[0].state);

    sleep(5);
    printf("Memoria liberada\n");

    // sem_memoria = sem_open("/sem_memoria", 0);
    // sem_bitacora = sem_open("/sem_bitacora", 0);
    // bitacora = fopen("bitacora.log", "a");

    // Free shared memory (The segment is not available JUST for this process anymore)
    if (shmdt(memoria) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
