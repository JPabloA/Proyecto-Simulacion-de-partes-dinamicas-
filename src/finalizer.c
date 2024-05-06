#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "./utilities/utilities.h"
#include "./utilities/sharedMemory.h"

int main(int argc, char const *argv[]) {

    releaseSharedMemorySegment(FILENAME, 's');
    releaseSharedMemorySegment(SHARED_INFO, 'a');
    releaseSharedMemorySegment(PROC_FILE, 'b');

    sem_t *sem = sem_open(SNAME, 0);

    if (sem == SEM_FAILED) {
        printf("ERROR: Failed getting semaphore - Finalizer\n");
        return 1;
    }

    int closeStatus = sem_close(sem);
    if (closeStatus < 0) {
        printf("Failed closing semaphore - Finalizer\n");
    }

    int unlinkStatus = sem_unlink(SNAME);
    if (unlinkStatus < 0) {
        printf("Failed unlinking semaphore - Finalizer\n");
        return 1;
    }

    printf("Semaphore released!\n");

    return 0;
}
