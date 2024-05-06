#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <semaphore.h>

#include "./utilities/utilities.h"
#include "./utilities/sharedMemory.h"
#include "./utilities/process_list.h"

sem_t semaphoreMemory;

void initSharedInformation(int shmid, int num_lines)
{
    printf("\n");
    SharedInformation *information = (SharedInformation *)attachSharedMemorySegment(shmid);

    printf("Setting shared information...\n");
    information[0].num_lines = num_lines;
    printf("Shared information set!\n");

    detachSharedMemorySegment(information);
}

void initMemoryLines(int shmid, int num_lines)
{
    printf("\n");
    Line *lines = (Line *)attachSharedMemorySegment(shmid);

    printf("Filling memory lines...\n");
    for (int i = 0; i < num_lines; ++i) {
        lines[i].state = InUse;
        lines[i].pid = 99;
    }
    // *** Para Worst Fit
    // for (int i = 1; i < 5; ++i) {
    //     lines[i].state = Available;
    //     lines[i].pid = -1;
    // }
    // for (int i = 7; i < num_lines; ++i) {
    //     lines[i].state = Available;
    //     lines[i].pid = -1;
    // }

    // *** Para Best Fit
    for (int i = 0; i < 8; ++i) {
        lines[i].state = Available;
        lines[i].pid = -1;
    }
    for (int i = 9; i < num_lines; ++i) {
        lines[i].state = Available;
        lines[i].pid = -1;
    }
    printf("Memory lines filled!\n");

    detachSharedMemorySegment(lines);
}

void initProcessList(int shmid) {
    printf("\n");
    Process_List *list = (Process_List*)attachSharedMemorySegment(shmid);

    printf("Filling process list by default...\n");
    for (int i = 0; i < MAX_LIST_LENGTH; i++) {
        list[i].proc = NULL;
        list[i].proc_state = NOT_DEFINED;
        list[i].state = EMPTY;
    }
    printf("Process list initialized!\n");

    detachSharedMemorySegment(list);
}

int main(int argc, char const *argv[])
{
    int num_lines;
    printf("Enter the number of memory lines: ");
    scanf("%d", &num_lines);

    // Calculate the necessary memory space
    int line_size = sizeof(Line); // * sizeof let me know the space that its needed for one line
    int memory_size = num_lines * line_size;

    sem_t *semaphoreMemory = sem_open(SNAME, O_CREAT, 0644, 1);
    if (semaphoreMemory == SEM_FAILED) {
        perror("Error al abrir el semáforo");
        exit(1);
    }

    int shmid1 = createSharedMemorySegment(FILENAME, 's', memory_size);
    int shmid2 = createSharedMemorySegment(SHARED_INFO, 'a', sizeof(SharedInformation));
    int shmid3 = createSharedMemorySegment(PROC_FILE, 'b', sizeof(Process_List) * MAX_LIST_LENGTH);

    initMemoryLines(shmid1, num_lines);
    initSharedInformation(shmid2, num_lines);
    initProcessList(shmid3);

    printf("\n > All shared memory segments set\n\n");

    return 0;
}
