#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "utilities.h"
#include "sharedMemory.c"

void initSharedInformation(int shmid, int num_lines) {
    printf("\n");
    SharedInformation* information = (SharedInformation*) attachSharedMemorySegment(shmid);

    printf("Setting shared information...\n");
    information[0].num_lines = num_lines;
    printf("Shared information set!\n");

    detachSharedMemorySegment(information);
}

void initMemoryLines(int shmid, int num_lines) {
    printf("\n");
    Line* lines = (Line*) attachSharedMemorySegment(shmid);

    printf("Filling memory lines...\n");
    for (int i = 0; i < num_lines; ++i) {
        lines[i].state = Available;
        lines[i].pid = -1;
        lines[i].time = 0;
    }
    printf("Memory lines filled!\n");

    detachSharedMemorySegment(lines);
}

int main(int argc, char const *argv[]) {
    int num_lines;
    printf("Enter the number of memory lines: ");
    scanf("%d", &num_lines);

    // // Calculate the necessary memory space
    int line_size = sizeof(Line); // * sizeof let me know the space that its needed for one line
    int memory_size = num_lines * line_size;

    int shmid1 = createSharedMemorySegment(FILENAME, 's', memory_size);
    int shmid2 = createSharedMemorySegment(SHARED_INFO, 'a', sizeof(SharedInformation));

    initMemoryLines(shmid1, num_lines);
    initSharedInformation(shmid2, num_lines);

    printf("\nAll shared memory segments set\n");

    return 0;
}
