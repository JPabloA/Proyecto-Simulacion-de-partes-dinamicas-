#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "utilities.h"
#include "sharedMemory.c"

int main(int argc, char const *argv[]) {

    releaseSharedMemorySegment(FILENAME, 's');
    releaseSharedMemorySegment(SHARED_INFO, 'a');

    sem_t *sem = sem_open(SNAME, 0);

    if (sem == SEM_FAILED) {
        printf("ERROR: Failed getting semaphore\n");
        return 1;
    }

    return 0;
}
