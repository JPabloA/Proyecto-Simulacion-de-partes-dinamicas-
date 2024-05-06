#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>

#include "./utilities/utilities.h"
#include "./utilities/sharedMemory.h"
#include "./utilities/process_list.h"

// Semaphores
sem_t *semaphoreMemory;

void showMemoryState(Line* memory, int lines) {
    sem_wait(semaphoreMemory);

    printf("\n Memory State:\n");
    printf(" +--------------------------\n");
    printf(" | Line\tProcID\tLine state\n");
    printf(" +--------------------------\n");
    for (int i = 0; i < lines; ++i) {
        printf(" | %d\t%d\t%s\n", i, memory[i].pid, (memory[i].state == Available) ? "Available" : "In Use");
    }
    printf(" +--------------------------\n\n");

    sem_post(semaphoreMemory);
}

void showProcessesStates(Process_List* list) {

    printf("\n Processes States:\n");
    printf("Processes With Memory Access:\n");
    printf("Processes Running:\n");
    printf("Processes Blocked:\n");
    return;
}

int main(int argc, char const *argv[]) {

    int shmid1 = getSharedMemorySegment(FILENAME, 's');
    int shmid2 = getSharedMemorySegment(SHARED_INFO, 'a');
    int shmid3 = getSharedMemorySegment(PROC_FILE, 'b');

    semaphoreMemory = sem_open(SNAME, 0);

    if (shmid1 < 0 || shmid2 < 0) {
        printf("Failed getting shared memory segment - Spy\n");
        return 1;
    }
    if (semaphoreMemory == SEM_FAILED) {
        printf("Failed opening memory semaphore - Spy\n");
        return 1;
    }

    Line* memory = (Line*) attachSharedMemorySegment(shmid1);
    SharedInformation* information = (SharedInformation*) attachSharedMemorySegment(shmid2);
    Process_List* list = (Process_List*) attachSharedMemorySegment(shmid3);

    showMemoryState(memory, information->num_lines);
    showProcessesStates(list);

    detachSharedMemorySegment(memory);
    detachSharedMemorySegment(information);
    detachSharedMemorySegment(list);

    return 0;
}
