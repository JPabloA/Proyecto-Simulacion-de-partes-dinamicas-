#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "./utilities/utilities.h"
#include "./utilities/sharedMemory.h"
#include "./utilities/sharedSemaphore.h"

// Share memory segments - Manager structures
SharedInformation *information;
int counter;

int main(int argc, char const *argv[]) {

    // To kill producer
    counter = 0;
    int shmid2 = getSharedMemorySegment(SHARED_INFO, 'a');
    if (shmid2 < 0)
    {
        printf("Failed getting segments in init environment\n");
        exit(1);
    }
    information = (SharedInformation *)attachSharedMemorySegment(shmid2);

    // flag to false to end the while
    information->flagForWhile = 0;

    while(information->flagForWhile == 0 && information->isProducerActive){
        sleep(5);
         printf("\033[1;33m (%d intento) Esperando a que el productor finalice...\033[0m\n", counter+1);

        // To avoid ctrl+c error in producer.c 
        if (counter >= 10){
            printf("\033[1;31mNo se ha podido establecer comunicación con el productor (Limpiando ambientes de ejecución)...\033[0m\n\n");
            break;
        }
        counter++;
    }
    //sleep(10);
    detachSharedMemorySegment(information);

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
