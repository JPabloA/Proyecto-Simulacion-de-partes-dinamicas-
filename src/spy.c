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

    Line* linesArray = (Line*) attachSharedMemorySegment(shmid1);
    SharedInformation* information = (SharedInformation*) attachSharedMemorySegment(shmid2);

    printf("linesArray  ==> pid: %d, state: %d\n", linesArray[0].pid, linesArray[0].state);
    printf("information ==> num_lines: %d\n", information[0].num_lines);

    if (shmdt(linesArray) == -1) {
        printf("Failed releasing linesArray memory\n");
        exit(1);
    }
    if (shmdt(information) == -1) {
        printf("Failed releasing information memory\n");
        exit(1);
    }
    printf("Read data succesfully\n");


    return 0;
}
