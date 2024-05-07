#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>

#include "./utilities/utilities.h"
#include "./utilities/sharedMemory.h"
#include "./utilities/process_list.h"
#include "./utilities/sharedSemaphore.h"

// Semaphores
sem_t *semaphoreMemory;
sem_t *semaphoreProcList;

void showMemoryState(Line* memory, int lines) {
    sem_wait(semaphoreMemory);

    printf("\n > Memory State:\n\n");
    printf(" +--------------------------\n");
    printf(" | Line\tProcID\tLine state\n");
    printf(" +--------------------------\n");
    for (int i = 0; i < lines; ++i) {
        printf(" | %d\t%d\t%s\n", i, memory[i].pid, (memory[i].state == Available) ? "Available" : "In Use");
    }
    printf(" +--------------------------\n\n");

    sem_post(semaphoreMemory);
}

void printProcesses(Process_List* list, char* print_title, char* status_text) {
    printf(" %s\n", print_title);
    printf(" +--------------------------\n");
    printf(" | ProcID\tProc State\n");
    printf(" +--------------------------\n");

    for (int i = 0; i < MAX_LIST_LENGTH && list[i].state != EMPTY; i++) {
        printf(" | %d\t%s\n", list[i].pid, (list[i].proc_state == WITH_MEMORY_ACCESS) ? status_text : "Is something wrong");
    }
    printf("\n");
    
}

void showProcessesStates(Process_List* list) {

    Process_List list_withMemoryAccess[MAX_LIST_LENGTH];
    Process_List list_procRunning[MAX_LIST_LENGTH];
    Process_List list_procBlocked[MAX_LIST_LENGTH];

    initProcessListByDefault(list_withMemoryAccess);
    initProcessListByDefault(list_procRunning);
    initProcessListByDefault(list_procBlocked);

    sem_wait(semaphoreProcList);

    // Set processes in each list based on their state
    for (int i = 0; i < MAX_LIST_LENGTH; ++i) {
        switch (list[i].proc_state) {
            case WITH_MEMORY_ACCESS:
                addProcessToList(list_withMemoryAccess, list[i].pid, WITH_MEMORY_ACCESS);
                break;
            case RUNNING:
                addProcessToList(list_procRunning, list[i].pid, RUNNING);
                break;
            case BLOCKED:
                addProcessToList(list_procBlocked, list[i].pid, BLOCKED);
                break;
            default:
                break;
        }
    }

    printf(" > Processes States:\n\n");
    printProcesses(list_withMemoryAccess, "Processes With Memory Access:", "With memory access");
    printProcesses(list_procRunning, "Processes Running:", "Running");
    printProcesses(list_procBlocked, "Blocked Processes:", "Blocked");

    sem_post(semaphoreProcList);
}

int main(int argc, char const *argv[]) {

    // Get shared semaphores
    semaphoreMemory = GetSemaphore(SNAME);
    semaphoreProcList = GetSemaphore(SNAME_PROC_LIST);

    if (semaphoreMemory == NULL || semaphoreProcList == NULL) {
        printf("Failed getting memory semaphores - Spy\n");
        return 1;
    }

    // Get shared memory segments
    int shmid1 = getSharedMemorySegment(FILENAME, 's');
    int shmid2 = getSharedMemorySegment(SHARED_INFO, 'a');
    int shmid3 = getSharedMemorySegment(PROC_FILE, 'b');

    if (shmid1 < 0 || shmid2 < 0 || shmid3 < 0) {
        printf("Failed getting shared memory segments - Spy\n");
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
