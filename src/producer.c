#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "./utilities/utilities.h"
#include "./utilities/process_list.h"
#include "./utilities/sharedMemory.h"
#include "./utilities/sharedSemaphore.h"

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
char* algorithm_name;

// Thread variable to keep creating proccesses
volatile short int keepRunning = 1;

// Load process info into memory
void setProcessInMemory(ThreadProcess* proc, int index) {
    for (int i = index; i < (index + proc->lines) && index != -1; ++i) {
        memory[i].state = InUse;
        memory[i].pid = proc->pid;
    }
}

int method_FirstFit(ThreadProcess *proc) {
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
            if (current_hole <= best_fit)
            {
                best_fit = current_hole; // best hole in this try
                hole_index = i;
            }
        }
        i += current_hole;
    }

    // If a suitable hole is found, load the process into memory
    setProcessInMemory(proc, hole_index);

    return hole_index;
}

int method_WorstFit(ThreadProcess *proc)
{
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
    setProcessInMemory(proc, hole_index);

    return hole_index;
}

void get_current_time(char* buffer, size_t size) 
{
    time_t raw_time = time(NULL);
    struct tm* time_info = localtime(&raw_time);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", time_info);
}

void write_to_bitacora(ThreadProcess *proc, char *action, char *type) 
{
    // Abrir el archivo de bitácora para agregar datos
    FILE* file = fopen("bitacora.txt", "a");
    if (file == NULL) {
        perror("Error al abrir la bitácora");
        exit(1);
    }

    BitacoraEntry entry;
    entry.pid = proc->pid; // Obtener el PID del proceso actual
    strcpy(entry.accion, action);
    strcpy(entry.tipo, type);
    
    get_current_time(entry.hora, sizeof(entry.hora));  // Obtener la hora actual
    entry.lineas = proc->lines;  // Número de líneas asignadas

    // Escribir la entrada en la bitácora
    fprintf(file, "ID: %d | Accion: %s | Tipo: %s | Hora: %s | Lineas: %d\n",
            entry.pid, entry.accion, entry.tipo, entry.hora, entry.lineas);

    // Cerrar el archivo y liberar el semáforo
    fclose(file);
}

int loadInSharedMemory(ThreadProcess *proc) {
    int index = -1;
    int posInList = -1;

    printf("\x1b[33m[Aplicando %s]: Proceso %d intentando adquirir semáforo de memoria\x1b[0m\n", algorithm_name, proc->pid);
    
    // ! to add the process info to the list
    sem_wait(semaphoreProcList);
    posInList = addProcessToList(processList, proc->pid, BLOCKED);
    proc->listIndex = posInList;
    sem_post(semaphoreProcList);

    sleep(10);
    sem_wait(semaphoreMemory);

    // !With Memory Access
    sem_wait(semaphoreProcList);
    changeProcState(processList, WITH_MEMORY_ACCESS, proc->listIndex);
    sem_post(semaphoreProcList);

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
    //sleep(45); //sleep to see spy.c working in different lists
    sleep(10);


    printf("\x1b[33m[Aplicando %s]: Proceso %d adquirió semáforo de memoria\x1b[0m\n", algorithm_name, proc->pid);
    printf("\x1b[33m[Aplicando %s]: El size del proceso %d es: %d\x1b[0m\n", algorithm_name, proc->pid, proc->lines);

    write_to_bitacora(proc, " Aplicando el metodo            ", "  Asignación   ");

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
    // Init pipe comunication
    mkfifo(PIPE_FILE, 0666);

    // Get semaphores
    semaphoreMemory = GetSemaphore(SNAME);
    semaphoreProcList = GetSemaphore(SNAME_PROC_LIST);

    if (semaphoreMemory == NULL) {
        printf("ERROR: Failed getting memory semaphore - Producer\n");
        printf("Make sure to execute the initializer before running the producer\n");
        exit(1);
    }
    if (semaphoreProcList == NULL) {
        printf("ERROR: Failed getting proc list semaphore - Producer\n");
        printf("Make sure to execute the initializer before running the producer\n");
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

    write_to_bitacora(proc, " Proceso Creado                 ", "  Asignación   ");

    // Assign process in memory
    int index = loadInSharedMemory(proc);

    if (index == -1)
    {
        printf("\x1b[31m>>> Process ID: %d couldnt be assigned -> End of process\x1b[0m\n", proc->pid);
        write_to_bitacora(proc, " No hay espacio para el proceso ", " Desasignacion ");

        // !Remove the process from the list (ND->Blocked->WMA->Running->ND)
        sem_wait(semaphoreProcList);
        removeProcessFromList(processList, proc->listIndex);  
        sem_post(semaphoreProcList);
        
        free(args);
        return NULL;
    }
    else{
        write_to_bitacora(proc, " Proceso Almacenado en Memoria  ", " Desasignacion ");
    }
    // !Running proccesses 
    sem_wait(semaphoreProcList);
    changeProcState (processList, RUNNING, proc->listIndex);
    sem_post(semaphoreProcList);

    sleep(proc->time);

    // !Remove the process from the list (ND->Blocked->WMA->Running->ND)
    sem_wait(semaphoreProcList);
    removeProcessFromList(processList, proc->listIndex);  
    sem_post(semaphoreProcList);

    // Released occupied memory lines
    printf("\x1b[32m>>> Process released: ID: %d - Lines: %d - Time: %d\x1b[0m\n", proc->pid, proc->lines, proc->time);
    write_to_bitacora(proc, " Proceso Liberado               ", " Desasignacion ");
    releaseInSharedMemory(index, proc);
    
    free(args);
    return NULL;
}

ThreadProcess *createProcess()
{
    ThreadProcess *args = (ThreadProcess *)malloc(sizeof(ThreadProcess));
    args->pid = getProccesID();
    args->lines = rand() % 10 + 1;
    args->listIndex = -1;
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
    while (keepRunning) {
        pthread_create(&thread, NULL, &searhForMemory, createProcess());

        int delay = rand() % 31 + 30;
        sleep(delay);
    }
}

// *** Comunication between prod and finalizer (Finish execution)
// Wait for the finalizer signal (Stop creating proccesses)
void *waitSignalFromFinalizer(void *arg) {
    char pipe_flags[] = {(char)1, (char)1};
    int pipeProducer;

    printf("\n[INFO]: Producer in execution. Waiting for finalizer signal...\n");
    while (pipe_flags[0]) {
        pipeProducer = open(PIPE_FILE, O_RDONLY);
        
        read(pipeProducer, pipe_flags, PIPE_SIZE);
        close(pipeProducer);

        sleep(3);
    }
    keepRunning = 0;
    printf("\n[INFO]: Finalizer signal received. Waiting all in-progress threads to finish\n\n");

    return NULL;
}

// Send signal to the finalizer (Indicates that producer has freed all structures)
void postSignalToFinalizer() {
    char pipe_flags[] = {(char)1, (char)1};
    pipe_flags[1] = (char)0;

    printf("\n[INFO]: Sending signal to finalizer...\n");

    int pipeProducer = open(PIPE_FILE, O_WRONLY);
    write(pipeProducer, pipe_flags, PIPE_SIZE);
    close(pipeProducer);
    
    printf("[INFO]: Signal to finalizer sended\n\n");
}

// Set the flags for producer and finalizer (Indicates that the producer is in execution)
void setPipeFlags() {
    char pipe_flags[] = {(char)1, (char)1};

    int pipeProducer = open(PIPE_FILE, O_WRONLY);
    write(pipeProducer, pipe_flags, PIPE_SIZE);
    close(pipeProducer);    
}

int main() {
    initEnvironment();

    // Solicitar el tipo de algoritmo
    printf("\nSeleccione el algoritmo de asignación de memoria:\n");
    printf("1. Best-Fit\n2. First-Fit\n3. Worst-Fit\n: ");

    int opcion;
    scanf("%d", &opcion);

    switch (opcion)
    {
    case 1:
        algorithm = BestFit;
        algorithm_name = "Best Fit";
        break;
    case 2:
        algorithm = FirstFit;
        algorithm_name = "First Fit";
        break;
    case 3:
        algorithm = WorstFit;
        algorithm_name = "Worst Fit";
        break;
    default:
        printf("Opción inválida\n");
        exit(1);
    }
    // Set all flags in pipe file
    setPipeFlags();

    // Thread that is going to wait for the finalizer signal
    pthread_t waitForFinalizer_Thread;
    pthread_create(&waitForFinalizer_Thread, NULL, waitSignalFromFinalizer, NULL);

    pthread_t processCreatorThread;
    pthread_create(&processCreatorThread, NULL, createProcesses, NULL);
    pthread_join(processCreatorThread, NULL);

    releaseEnvironment();

    // Send signal to finalizer (The producer freed all its structures)
    postSignalToFinalizer();

    return 0;
}
