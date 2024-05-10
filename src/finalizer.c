#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "./utilities/utilities.h"
#include "./utilities/sharedMemory.h"
#include "./utilities/sharedSemaphore.h"

// Check if the pipe comunicator file is empty := Is not neccessary to wait for signal
short int isFileEmpty() {
    FILE* file;
    file = fopen(PIPE_FILE, "r");

    if (file == NULL) {
        printf("ERROR: Failed getting pipe file comunicator\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    short int isEmpty = ftell(file) == 0;
    fclose(file);

    return isEmpty;
}

// Send signal to the producer (Stop creating processes)
void postSignalToProducer() {
    char pipe_flags[] = {(char)1, (char)1};
    pipe_flags[0] = (char)0;

    printf("[INFO]: Sending signal to producer...\n");

    int pipeFinalizer = open(PIPE_FILE, O_WRONLY);
    write(pipeFinalizer, pipe_flags, PIPE_SIZE);
    close(pipeFinalizer);
    
    printf("[INFO]: Signal to producer sended\n");
}

// Wait for producer signal (Wait until producer has freed all structures)
void waitSignalFromProducer() {
    char pipe_flags[] = {(char)1, (char)1};
    int pipeFinalizer;

    printf("[INFO]: Waiting for producer signal...\n");
    
    while (pipe_flags[1]) {
        pipeFinalizer = open(PIPE_FILE, O_RDONLY);
        
        read(pipeFinalizer, pipe_flags, PIPE_SIZE);
        close(pipeFinalizer);

        sleep(3);
    };
    printf("[INFO]: Producer signal received. Freeing all shared segments\n");
}

int main(int argc, char const *argv[]) {

    // Init pipe comunication
    mkfifo(PIPE_FILE, 0666);

    if (!isFileEmpty()) {
        // Send signal to finish producer
        postSignalToProducer();
        // Wait signal from producer (It has finished)
        waitSignalFromProducer();
    }

    releaseSharedMemorySegment(FILENAME, 's');
    releaseSharedMemorySegment(SHARED_INFO, 'a');
    releaseSharedMemorySegment(PROC_FILE, 'b');

    // Get shared semaphores
    sem_t* semaphoreMemory = GetSemaphore(SNAME);
    sem_t* semaphoreProcList = GetSemaphore(SNAME_PROC_LIST);

    if (semaphoreMemory == NULL) {
        printf("ERROR: Failed getting memory semaphore - Finalizer\n");
    }
    else {
        CloseSemaphore(semaphoreMemory);
        UnlinkSemaphore(SNAME);
    }

    if (semaphoreProcList == NULL) {
        printf("ERROR: Failed getting proc list semaphore - Finalizer\n");
    }
    else {
        CloseSemaphore(semaphoreProcList);
        UnlinkSemaphore(SNAME_PROC_LIST);
    }

    // Clear content of file
    if (fclose(fopen(PIPE_FILE, "w")) != 0) {
        printf("ERROR: Failed removing pipeFile comunicator content\n");
    }

    FILE* file = fopen("bitacora.txt", "a");
    if (file == NULL) {
        perror("Error al abrir la bitácora");
        exit(1);
    }

    // Escribir la entrada en la bitácora
    fprintf(file, "*****  Finalizacion del programa, Memoria Liberada! *****\n");

    // Cerrar el archivo y liberar el semáforo
    fclose(file);

    return 0;
}
