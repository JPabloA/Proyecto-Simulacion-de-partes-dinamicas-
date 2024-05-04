#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include "utilities.h"
#include "sharedMemory.h"

// Thread Pool implementation
#define THREAD_NUMBER 5
pthread_t threads[THREAD_NUMBER];

Line *memory;
SharedInformation *information;

sem_t *semaphoreMemory, *semaphoreLog;
FILE *log;

int num_lines;
int currentProccesNumber;

enum Algorithm algorithm;

int method_FirstFit(ThreadProcess *proc) {
    int index = -1;
    int proc_size = proc->lines;

    // Check if spaces are available
    for (int i = 0; i < num_lines; ++i) {
        if (memory[i].state == Available) {
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
        for (int i = index; i < (index + proc->lines); ++i) {
            memory[i].state = InUse;
            memory[i].pid = proc->pid;
        }
        return index;
    }
    return -1;
}

int loadInSharedMemory(ThreadProcess *proc) {

    int index = -1;

    printf("Proceso %d intentando adquirir semáforo de memoria\n", proc->pid);
    sem_wait(semaphoreMemory);

    if (algorithm == FirstFit) {
        index = method_FirstFit(proc);
    }
    else if (algorithm == BestFit) {
        index = method_FirstFit(proc);
    }
    else {
        index = method_FirstFit(proc);
    }

    sem_post(semaphoreMemory);
    printf("Proceso %d adquirió semáforo de memoria\n", proc->pid);
    printf("El size del proceso es: %d\n", proc->lines);

    return index;
}

void releaseInSharedMemory(int index, ThreadProcess *proc) {
    sem_wait(semaphoreMemory);

    // Release asigned memory lines
    for (int i = index; i < proc->lines; ++i) {
        memory[i].state = Available;
        memory[i].pid = -1;
    }

    sem_post(semaphoreMemory);
}

void initEnvironment() {
    int shmid1 = getSharedMemorySegment(FILENAME, 's');
    int shmid2 = getSharedMemorySegment(SHARED_INFO, 'a');
    semaphoreMemory = sem_open(SNAME, 0);

    if (shmid1 < 0 || shmid2 < 0) {
        printf("Failed getting segments in init environment");
        exit(1);
    }

    if (semaphoreMemory == SEM_FAILED) {
        printf("Error al abrir el semáforo\n");
        exit(1);
    }

    memory = (Line *)attachSharedMemorySegment(shmid1);
    information = (SharedInformation *)attachSharedMemorySegment(shmid2);

    num_lines = information->num_lines;
}

void releaseEnvironment() {
    detachSharedMemorySegment(memory);
    detachSharedMemorySegment(information);
}

int getProccesID() {
    return currentProccesNumber;
}

void *searhForMemory(void* args) {
    ThreadProcess* proc = (ThreadProcess*) args;
    printf("> Process created: ID: %d - Lines: %d - Time: %d\n", proc->pid, proc->lines, proc->time);

    // Assign process in memory
    int index = loadInSharedMemory(proc);

    if (index == -1) {
        printf("> Process couldnt be assigned -> End of process\n");

        free(args);
        pthread_exit(NULL);
        return NULL;
    }
    sleep(proc->time);

    // Released occupied memory lines
    printf("> Process released: ID: %d - Lines: %d - Time: %d\n", proc->pid, proc->lines, proc->time);
    releaseInSharedMemory(index, proc);

    free(args);
    pthread_exit(NULL);
    return NULL;
}

ThreadProcess* createProcess() {
    ThreadProcess* args = (ThreadProcess*) malloc( sizeof( ThreadProcess ) );
    args->pid = getProccesID();
    args->lines = rand() % 10 + 1;
    args->time = rand() % 41 + 20;
    
    // *** Mutex para el currentProcessNumber???
    currentProccesNumber++;

    return args;
}

void *createProcesses(void *arg) {
    pthread_t thread;

    // !: while to create the process
    printf("\n");
    printf("\n");
    srand(time(NULL));
    while (true) {

        pthread_create(&thread, NULL, &searhForMemory, createProcess());
        // pthread_join(hilo, NULL);

        // int delay = rand() % 31 + 30;
        int delay = 10;
        printf("Main sleeping for %d seconds...\b", delay);
        sleep(delay);
    }
}

int main() {
    // Solicitar el tipo de algorithmexit
    printf("Seleccione el algorithm de asignación de memoria:\n");
    printf("1. Best-Fit\n2. First-Fit\n3. Worst-Fit\n");

    int opcion;
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            algorithm = BestFit;
            break;
        case 2:
            algorithm = FirstFit;
            break;
        case 3:
            algorithm = WorstFit;
            break;
        default:
            printf("Opción inválida\n");
            exit(1);
    }

    initEnvironment();

    // // Create all pthreads 
    // for (int i = 0; i < THREAD_NUMBER; i++) {
    //     pthread_create(&threads[i], NULL, &startThread, NULL);
    // }  
    // // Join all pthreads 
    // for (int i = 0; i < THREAD_NUMBER; i++) {
    //     pthread_join(threads[0], NULL);
    // }

    pthread_t processCreatorThread;
    pthread_create(&processCreatorThread, NULL, createProcesses, NULL);
    pthread_join(processCreatorThread, NULL);

    releaseEnvironment();

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

    // sem_memoria = sem_open("/sem_memoria", 0);
    // sem_log = sem_open("/sem_log", 0);
    // log = fopen("log.log", "a");

    return 0;
}
