#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "./utilities/utilities.h"
#include "./utilities/sharedMemory.h"
#include "./utilities/sharedSemaphore.h"

int main(int argc, char const *argv[]) {

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
