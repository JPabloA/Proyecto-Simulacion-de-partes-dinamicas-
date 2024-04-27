#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "utilities.h"

// Save basic information into shared memory segment
void saveInformation(int num_lines) {
    // Set the key to the memory segment
    key_t key = ftok(SHARED_INFO, 'c');

    // Create shared memory by the size of the struct MemoryCell
    int shmid = shmget(key, sizeof(BasicInformation), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory to the process
    BasicInformation* memory = (BasicInformation*) shmat(shmid, NULL, 0);
    if (memory == (BasicInformation*) -1) {
        perror("shmat");
        exit(1);
    }

    // Initialize the shared memory
    memory[0].num_lines = num_lines;

    // Free shared memory (The segment is not available JUST for this process anymore)
    if (shmdt(memory) == -1) {
        perror("shmdt");
        exit(1);
    }
}

int main(int argc, char const *argv[]) {
    int num_lines;
    printf("Enter the number of memory lines: ");
    scanf("%d", &num_lines);

    // Calculate the necessary memory space
    int line_size = sizeof(Line); // * sizeof let me know the space that its needed for one line
    int memory_size = num_lines * line_size;

    // saveInformation(num_lines);

    // Set the key to the memory segment
    key_t key = ftok(FILENAME, 's');
    int shmid;

    // Create shared memory by the size of the struct MemoryCell
    shmid = shmget(key, memory_size, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory to the process
    Line* memory = (Line*) shmat(shmid, NULL, 0);
    if (memory == (Line*) -1) {
        perror("shmat");
        exit(1);
    }

    // Initialize the shared memory
    for (int i = 0; i < num_lines; i++) {
        memory[i].state = Available;
        memory[i].pid = -1;
        memory[i].time = 0;
    }

    printf("Memory shared with ID: %d and %d bytes\n", shmid, memory_size);
    sleep(5);

    // Free shared memory (The segment is not available JUST for this process anymore)
    if (shmdt(memory) == -1) {
        perror("shmdt");
        exit(1);
    }

    printf("Shared memory segment free and released\n");

    return 0;
}
