#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include "utilities.h"

#define FILENAME "./sharedFile"

int numLineas;
Line *memoria;
sem_t *sem_memoria, *sem_bitacora;
FILE *bitacora;

void asignarMemoria(int pid, int tamano, enum Algoritmo algoritmo);
void liberarMemoria(int pid);

int main() {
    // Solicitar el tipo de algoritmo
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

    key_t key = ftok(FILENAME, 's');
    int shmid;

    // Obtener acceso a la memoria compartida y los semáforos
    int shmid = shmget(key, 0, 0);
    if (shmid == -1) {
        perror("Error al obtener la memoria compartida");
        exit(1);
    }
    memoria = shmat(shmid, NULL, 0);
    numLineas = shmid / sizeof(Line);

    // sem_memoria = sem_open("/sem_memoria", 0);
    // sem_bitacora = sem_open("/sem_bitacora", 0);
    // bitacora = fopen("bitacora.log", "a");

    // Generar procesos
    srand(time(NULL));
    while (1) {
        int tamano = rand() % 10 + 1;
        int tiempo = rand() % 41 + 20;
        int pid = getpid();

        asignarMemoria(pid, tamano, algoritmo);
        sleep(tiempo);
        liberarMemoria(pid);

        int delay = rand() % 31 + 30;
        sleep(delay);
    }

    return 0;
}

void asignarMemoria(int pid, int size, enum Algoritmo algoritmo) {
    sem_wait(sem_memoria);

    int asignado = 0;
    for (int i = 0; i <= numLineas - size; i++) {
        int hueco = 1;
        for (int j = 0; j < size; j++) {
            if (memoria[i + j].state == InUse) {
                hueco = 0;
                i += j;
                break;
            }
        }
        if (hueco) {
            for (int j = 0; j < size; j++) {
                memoria[i + j].state = InUse;
                memoria[i + j].pid = pid;
            }
            asignado = 1;
            break;
        }
    }

    // if (asignado) {
    //     sem_wait(sem_bitacora);
    //     fprintf(bitacora, "Proceso %d asignado (%d líneas)\n", pid, size);
    //     sem_post(sem_bitacora);
    // } else {
    //     sem_wait(sem_bitacora);
    //     fprintf(bitacora, "Proceso %d no pudo ser asignado\n", pid);
    //     sem_post(sem_bitacora);
    // }

    // sem_post(sem_memoria);
}

void liberarMemoria(int pid) {
    sem_wait(sem_memoria);

    // Liberar la memoria asignada al proceso
    // ...

    sem_post(sem_memoria);
}