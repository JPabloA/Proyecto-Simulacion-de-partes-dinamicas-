#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include "utilities.h"
#include "sharedMemory.c"

// Semaphores
sem_t *semaphoreMemory;

void showMemoryState(Line* memory, int lines) {
    sem_wait(semaphoreMemory);

    for (int i = 0; i < lines; ++i) {
        printf("Line %d: ID=%d - State=%s\n", i, memory[i].pid, (memory[i].state == Available) ? "Available" : "In Use");
    }

    sem_post(semaphoreMemory);
}

int main(int argc, char const *argv[]) {

    int shmid1 = getSharedMemorySegment(FILENAME, 's');
    int shmid2 = getSharedMemorySegment(SHARED_INFO, 'a');
    semaphoreMemory = sem_open(SNAME, 0);

    if (shmid1 < 0 || shmid2 < 0) {
        printf("Failed getting shared memory segment - Spy");
        return 1;
    }
    if (semaphoreMemory == SEM_FAILED) {
        printf("Failed opening memory semaphore - Spy\n");
        return 1;
    }

    Line* memory = (Line*) attachSharedMemorySegment(shmid1);
    SharedInformation* information = (SharedInformation*) attachSharedMemorySegment(shmid2);

    showMemoryState(memory, information->num_lines);

    detachSharedMemorySegment(memory);
    detachSharedMemorySegment(information);

    printf("Read data succesfully\n");

    return 0;
}
