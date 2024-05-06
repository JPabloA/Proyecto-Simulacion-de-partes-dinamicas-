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

    if (semaphoreMemory == NULL || semaphoreProcList == NULL) {
        printf("ERROR: Failed getting semaphore - Finalizer\n");
        return 1;
    }

    CloseSemaphore(semaphoreMemory);
    CloseSemaphore(semaphoreProcList);

    UnlinkSemaphore(SNAME);
    UnlinkSemaphore(SNAME_PROC_LIST);

    return 0;
}
