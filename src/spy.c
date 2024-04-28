#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include "utilities.h"
#include "sharedMemory.c"

int main(int argc, char const *argv[]) {

    int shmid1 = getSharedMemorySegment(FILENAME, 's');
    int shmid2 = getSharedMemorySegment(SHARED_INFO, 'a');

    if (shmid1 < 0 || shmid2 < 0) {
        printf("Failed getting shared memory segment - Spy");
        return 1;
    }

    Line* linesArray = (Line*) attachSharedMemorySegment(shmid1);
    SharedInformation* information = (SharedInformation*) attachSharedMemorySegment(shmid2);

    for (int i = 0; i < information->num_lines; ++i) {
        printf("Line %d: ID=%d - State=%s\n", i, linesArray[i].pid, (linesArray[i].state == Available) ? "Available" : "In Use");
    }

    printf("linesArray  ==> pid: %d, state: %d\n", linesArray[0].pid, linesArray[0].state);
    printf("information ==> num_lines: %d\n", information[0].num_lines);

    detachSharedMemorySegment(linesArray);
    detachSharedMemorySegment(information);

    printf("Read data succesfully\n");

    return 0;
}
