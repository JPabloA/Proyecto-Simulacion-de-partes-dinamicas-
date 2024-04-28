#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include "utilities.h"
#include "sharedMemory.c"

Line *memory;
SharedInformation* information;

sem_t *sem_memoria, *sem_bitacora;
FILE *bitacora;

int num_lines;


int loadInSharedMemory(ThreadProcess* proc) {
    int index = -1;
    int proc_size = proc->lines;

    // *** AQUI TIENE QUE IR UN SEMAFORO
    // *** El check de spaces depende del algoritmo -> Este es el first fit

    // Check if spaces are available
    for (int i = 0; i < num_lines; ++i ) {
        if ( memory[i].state == Available ) {
            proc_size--;
        }
        else {
            proc_size = proc->lines;
            continue;
        }

        if (proc_size == 0) {
            index = i - proc->lines + 1;
            break;
        }
    }

    // If proc_size == 0 :: Space found
    if (!proc_size && index != -1) {
        // Load process in memory
        for (int i = index; i < proc->lines; ++i ) {
            memory[i].state = InUse;
            memory[i].pid = proc->pid;
        }

        return index;
    }

    // *** AQUI TIENE QUE IR UN SEMAFORO

    return -1;
}

void releaseInSharedMemory(int index, ThreadProcess* proc) {
    // *** AQUI TIENE QUE IR UN SEMAFORO

    // Release asigned memory lines
    for (int i = index; i < proc->lines; ++i ) {
        memory[i].state = Available;
        memory[i].pid = -1;
    }

    // *** AQUI TIENE QUE IR UN SEMAFORO
}


void generateThreadProcess() {
    srand(time(NULL));

    // Init a ThreadProcess struct with time, lines and pid
    ThreadProcess proc;
    proc.lines = rand() % 10 + 1;
    proc.time  = rand() % 41 + 20;
    proc.pid = getpid();

    // Assign process in memory
    int index = loadInSharedMemory(&proc);

    if (index == -1) {
        printf("Process couldnt be assigned -> End of process\n");
        return;
    }

    // Released occupied memory lines
    releaseInSharedMemory(index, &proc);
}

void initEnvironment() {
    int shmid1 = getSharedMemorySegment(FILENAME, 's');
    int shmid2 = getSharedMemorySegment(SHARED_INFO, 'a');
    
    if (shmid1 < 0 || shmid2 < 0) {
        printf("Failed getting segments in init environment");
        exit(1);
    }

    memory = (Line*) attachSharedMemorySegment(shmid1);
    information = (SharedInformation*) attachSharedMemorySegment(shmid2);

    num_lines = information->num_lines;
}

void releaseEnvironment() {
    detachSharedMemorySegment(memory);
    detachSharedMemorySegment(information);
}

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

    initEnvironment();

    memory[0].pid = 5;
    memory[0].state = Available;

    information[0].num_lines = 100;
    
    releaseEnvironment();
    printf("Wrote data succesfully\n");

    // memoria = (Line*) shmat(shmid, NULL, 0);
    // num_lines = shmid / sizeof(Line);

    // getSharedInformation();

    // printf("Memoria asignada...\n");
    // memoria[0].pid = 5;
    // memoria[0].state = InUse;
    // sleep(5);
    // printf("Memoria liberada\n");
    // printf("ID %d\n", shmid);
    // printf("Num lineas %d\n", num_lines);

    // // sem_memoria = sem_open("/sem_memoria", 0);
    // // sem_bitacora = sem_open("/sem_bitacora", 0);
    // // bitacora = fopen("bitacora.log", "a");

    // // Free shared memory (The segment is not available JUST for this process anymore)
    // if (shmdt(memoria) == -1) {
    //     perror("shmdt");
    //     exit(1);
    // }

    return 0;
}

// void asignarMemoria(int pid, int size, enum Algoritmo algoritmo) {
//     sem_wait(sem_memoria);

//     int asignado = 0;
//     for (int i = 0; i <= num_lines - size; i++) {
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