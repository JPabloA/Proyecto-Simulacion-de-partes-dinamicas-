#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#include "./utilities/utilities.h"
#include "./utilities/process_list.h"
#include "./utilities/sharedMemory.h"
#include "./utilities/sharedSemaphore.h"

// Thread Pool implementation
#define THREAD_NUMBER 5
pthread_t threads[THREAD_NUMBER];

// Share memory segments - Manager structures
Line *memory;
SharedInformation *information;
Process_List *processList;

// Share semaphores
sem_t *semaphoreMemory, *semaphoreLog, *semaphoreProcList;

// Global variables
int num_lines;
int currentProccesNumber;
enum Algorithm algorithm;

void setProcessInMemory(ThreadProcess* proc, int index) {
    for (int i = index; i < (index + proc->lines) && index != -1; ++i) {
        memory[i].state = InUse;
        memory[i].pid = proc->pid;
    }
}

int method_FirstFit(ThreadProcess *proc)
{
    int index = -1;
    int proc_size = proc->lines;

    // Check if spaces are available
    for (int i = 0; i < num_lines; ++i) {
        proc_size = (memory[i].state == Available) ? proc_size - 1 : proc->lines;

        if (proc_size == 0) {
            index = i - proc->lines + 1;
            break;
        }
    }

    setProcessInMemory(proc, index);
    return index;
}

int method_BestFit(ThreadProcess *proc)
{
    int index = -1;
    int proc_size = proc->lines;
    int best_fit = num_lines; // Initialize with the maximum possible value
    int hole_index = -1;

    // Find the smallest hole that can accommodate the process
    for (int i = 0; i < num_lines; ++i)
    {
        int current_hole = 0;

        if(memory[i].state != Available){
            continue;
        }

        // to check next lines
        for (int j = i; j < num_lines; ++j)
        {
            if (memory[j].state == Available)
            {
                current_hole++;
            }
            else
            {
                break;
            }
        }

        if (current_hole >= proc_size) // Only InUse space are gonna get here (current hole only are gonna be != than 0 if was empty spaces before)
        {
            if (current_hole < best_fit)
            {
                best_fit = current_hole; // best hole in this try
                hole_index = i;
            }
        }
        i += current_hole;
    }

    // If a suitable hole is found, load the process into memory
    index = hole_index;
    for (int i = index; i < (index + proc->lines) && index != -1; ++i)
    {
        memory[i].state = InUse;
        memory[i].pid = proc->pid;
    }

    return index;
}

int method_WorstFit(ThreadProcess *proc)
{
    int index = -1;
    int proc_size = proc->lines;
    int worst_fit = 1; // Initialize with the min possible value
    int hole_index = -1;

    // Find the largest hole that can accommodate the process
    for (int i = 0; i < num_lines; ++i)
    {
        int current_hole = 0;

        if(memory[i].state != Available){
            continue;
        }

        // to check next lines
        for (int j = i; j < num_lines; ++j)
        {
            if (memory[j].state == Available)
            {
                current_hole++;
            }
            else
            {
                break;
            }
        }

        if (current_hole >= proc_size) // Only InUse space are gonna get here (current hole only are gonna be != than 0 if was empty spaces before)
        {
            if (current_hole > worst_fit)
            {
                worst_fit = current_hole; // best hole in this try
                hole_index = i;
            }
        }
        i += current_hole;
    }

    // If a suitable hole is found, load the process into memory
    index = hole_index;
    for (int i = index; i < (index + proc->lines) && index != -1; ++i)
    {
        memory[i].state = InUse;
        memory[i].pid = proc->pid;
    }

    return index;
}

int loadInSharedMemory(ThreadProcess *proc)
{

    int index = -1;

    printf("[Aplicando metodo]: Proceso %d intentando adquirir semáforo de memoria\n", proc->pid);
    sem_wait(semaphoreMemory);

    // !With Memory Access
    // ? WE NEED TO ADD THE PROCESS TO THE LIST HERE (NO & because they are pointers)
    //addProcessToList(processList, proc, WITH_MEMORY_ACCESS);

    if (algorithm == FirstFit)
    {
        index = method_FirstFit(proc);
    }
    else if (algorithm == BestFit)
    {
        index = method_BestFit(proc);
    }
    else
    {
        index = method_WorstFit(proc);
    }

    sem_post(semaphoreMemory);

    // here is where it supposed to be the remove but the next state of the process is running so we only change the state

    printf("[Aplicando metodo]: Proceso %d adquirió semáforo de memoria\n", proc->pid);
    printf("[Aplicando metodo]: El size del proceso %d es: %d\n", proc->pid, proc->lines);

    return index;
}

void releaseInSharedMemory(int index, ThreadProcess *proc)
{
    sem_wait(semaphoreMemory);

    // Release asigned memory lines
    for (int i = index; i < (index + proc->lines); ++i) 
    {
        memory[i].state = Available;
        memory[i].pid = -1;
    }

    sem_post(semaphoreMemory);
}

void initEnvironment() {
    // Get semaphores
    semaphoreMemory = GetSemaphore(SNAME);
    semaphoreProcList = GetSemaphore(SNAME_PROC_LIST);

    if (semaphoreMemory == NULL) {
        printf("ERROR: Failed getting memory semaphore - Producer\n");
        exit(1);
    }
    if (semaphoreProcList == NULL) {
        printf("ERROR: Failed getting proc list semaphore - Producer\n");
        exit(1);
    }

    int shmid1 = getSharedMemorySegment(FILENAME, 's');
    int shmid2 = getSharedMemorySegment(SHARED_INFO, 'a');
    int shmid3 = getSharedMemorySegment(PROC_FILE, 'b');

    if (shmid1 < 0 || shmid2 < 0 || shmid3 < 0)
    {
        printf("Failed getting segments in init environment");
        exit(1);
    }

    memory = (Line *)attachSharedMemorySegment(shmid1);
    information = (SharedInformation *)attachSharedMemorySegment(shmid2);
    processList = (Process_List *)attachSharedMemorySegment(shmid3);

    num_lines = information->num_lines;
}

void releaseEnvironment()
{
    detachSharedMemorySegment(memory);
    detachSharedMemorySegment(information);
    detachSharedMemorySegment(processList);
}

int getProccesID()
{
    return currentProccesNumber;
}

void *searhForMemory(void *args)
{
    ThreadProcess *proc = (ThreadProcess *)args;
    printf(">>> Process created: ID: %d - Lines: %d - Time: %d\n", proc->pid, proc->lines, proc->time);

    // Assign process in memory
    int index = loadInSharedMemory(proc);

    if (index == -1)
    {
        printf(">>> Process couldnt be assigned -> End of process\n");

        free(args);
        pthread_exit(NULL);
        return NULL;
    }
    // !Running proccesses 
    changeProcState (processList, RUNNING, proc->listIndex);
    sleep(proc->time);

    // !Remove the process from the list (ND->Blocked->WMA->Running->ND)
    if (proc->listIndex != 0 ) {
        removeProcessFromList(processList, proc->listIndex);       
    }

    // Released occupied memory lines
    printf(">>> Process released: ID: %d - Lines: %d - Time: %d\n", proc->pid, proc->lines, proc->time);
    releaseInSharedMemory(index, proc);
    free(args);
    pthread_exit(NULL);
    return NULL;
}

ThreadProcess *createProcess()
{
    ThreadProcess *args = (ThreadProcess *)malloc(sizeof(ThreadProcess));
    args->pid = getProccesID();
    args->lines = rand() % 10 + 1;
    // args->lines = 3;
    args->time = rand() % 41 + 20;
    currentProccesNumber++;

    return args;
}

void *createProcesses(void *arg)
{
    printf("\n\n");
    pthread_t thread;

    // !: while to create the process
    srand(time(NULL));
    while (true)
    {

        pthread_create(&thread, NULL, &searhForMemory, createProcess());
        // pthread_join(thread, NULL);

        int delay = rand() % 31 + 30;
        delay = 10;
        sleep(delay);
    }
}

int main()
{
    // Solicitar el tipo de algorithmexit
    printf("Seleccione el algorithm de asignación de memoria:\n");
    printf("1. Best-Fit\n2. First-Fit\n3. Worst-Fit\n");

    int opcion;
    scanf("%d", &opcion);

    switch (opcion)
    {
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
