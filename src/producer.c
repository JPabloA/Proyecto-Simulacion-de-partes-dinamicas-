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
#include "sharedMemory.c"

// Thread Pool implementation
#define THREAD_NUMBER 5
pthread_t threads[THREAD_NUMBER];

Line *memory;
SharedInformation *information;

pthread_t hilo;
ThreadProcess args;

sem_t *semaphoreMemory, *semaphoreLog;
FILE *bitacora;

int num_lines;
int currentProccesNumber, ProcessTime, ProcessSize;

int loadInSharedMemory(ThreadProcess *proc)
{

    int index = -1;
    int proc_size = proc->lines;

    printf("Proceso %d intentando adquirir semáforo de memoria\n", proc->pid);
    sem_wait(semaphoreMemory);
    printf("Proceso %d adquirió semáforo de memoria\n", proc->pid);
    printf("El size del proceso es: %d\n", proc->lines);
    // *** El check de spaces depende del algoritmo -> Este es el first fit

    // Check if spaces are available
    for (int i = 0; i < num_lines; ++i)
    {
        if (memory[i].state == Available)
        {
            proc_size--;
        }
        else
        {
            proc_size = proc->lines;
            continue;
        }

        if (proc_size == 0)
        {
            index = i - proc->lines + 1;
            break;
        }
    }

    // If proc_size == 0 :: Space found
    if (!proc_size && index != -1)
    {
        // Load process in memory
        for (int i = index; i < proc->lines; ++i)
        {
            memory[i].state = InUse;
            memory[i].pid = proc->pid;
        }
        sem_post(semaphoreMemory);
        return index;
    }

    sem_post(semaphoreMemory);

    return -1;
}

void releaseInSharedMemory(int index, ThreadProcess *proc)
{
    // *** AQUI TIENE QUE IR UN SEMAFORO
    sem_wait(semaphoreMemory);

    // Release asigned memory lines
    for (int i = index; i < proc->lines; ++i)
    {
        memory[i].state = Available;
        memory[i].pid = -1;
    }

    // *** AQUI TIENE QUE IR UN SEMAFORO
    sem_post(semaphoreMemory);
}

void initEnvironment()
{
    int shmid1 = getSharedMemorySegment(FILENAME, 's');
    int shmid2 = getSharedMemorySegment(SHARED_INFO, 'a');

    if (shmid1 < 0 || shmid2 < 0)
    {
        printf("Failed getting segments in init environment");
        exit(1);
    }

    memory = (Line *)attachSharedMemorySegment(shmid1);
    information = (SharedInformation *)attachSharedMemorySegment(shmid2);

    num_lines = information->num_lines;
}

void releaseEnvironment()
{
    detachSharedMemorySegment(memory);
    detachSharedMemorySegment(information);
}

int getProccesID()
{
    return currentProccesNumber;
}

void *searhForMemory()
{
    // Assign process in memory
    int index = loadInSharedMemory(&args);

    if (index == -1)
    {
        printf("Process couldnt be assigned -> End of process\n");
        pthread_exit(NULL);
        return NULL;
    }
    printf("Thread created: ID: %d - Lines: %d - Time: %d\n", args.pid, args.lines, args.time);
    sleep(args.time);

    // Released occupied memory lines
    releaseInSharedMemory(index, &args);

    pthread_exit(NULL);
    return NULL;
}

void *createProcesses(void *arg)
{
    // !: while to create the process
    srand(time(NULL));
    while (true)
    {

        args.pid = getProccesID();
        currentProccesNumber++;
        args.lines = rand() % 10 + 1;
        args.time = rand() % 41 + 20;

        pthread_create(&hilo, NULL, &searhForMemory, NULL);
        // pthread_join(hilo, NULL);

        int delay = rand() % 31 + 30;
        sleep(delay);
    }
}

int main()
{
    // Solicitar el tipo de algoritmoexit
    enum Algoritmo algoritmo;

    printf("Seleccione el algoritmo de asignación de memoria:\n");
    printf("1. Best-Fit\n2. First-Fit\n3. Worst-Fit\n");

    int opcion;
    scanf("%d", &opcion);

    switch (opcion)
    {
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

    // !: abrir semaforo
    semaphoreMemory = sem_open(SNAME, 0);
    if (semaphoreMemory == SEM_FAILED)
    {
        printf("Error al abrir el semáforo\n");
        exit(1);
    }

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

    return 0;
}
