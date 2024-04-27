#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include "utilities.h"

#define SHM_KEY 0x12345
#define SEM_KEY 0x54321

int main() {
    int num_lines;
    printf("Enter the number of memory lines: ");
    scanf("%d", &num_lines);

    // Calculate the necessary memory space
    int line_size = sizeof(Line); // * sizeof let me know the space that its needed for one line
    int memory_size = num_lines * line_size;

    // Create shared memory for memory simulation
    int shmid = shmget(SHM_KEY, memory_size, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error creating shared memory");
        exit(1);
    }

    // Initialize the shared memory
    Line *memory = shmat(shmid, NULL, 0);
    for (int i = 0; i < num_lines; i++) {
        memory[i].state = Available;
        memory[i].pid = -1;
        memory[i].time = 0;
    }

    printf("Memory shared with ID: %d and %ld bytes\n", shmid, num_lines * sizeof(Line));

    // Free shared memory (The segment is not available for JUST this process anymore)
    if (shmdt(memory) == -1) {
        perror("shmdt");
        exit(1);
    }

    // // Create semaphores for synchronization
    // sem_t *sem_memory = sem_open("/sem_memory", O_CREAT | O_EXCL, 0644, 1);
    // sem_t *sem_log = sem_open("/sem_log", O_CREAT | O_EXCL, 0644, 1);

    // // Open the log file
    // FILE *log_file = fopen("log.log", "w");
    // if (log_file == NULL) {
    //     perror("Error opening log file");
    //     exit(1);
    // }

    // Print initialization information
    printf("Shared memory and semaphores initialized.\n");
    // fprintf(log_file, "Simulation started.\n");
    // fprintf(log_file, "Number of memory lines: %d\n", num_lines);
    // fprintf(log_file, "Allocated memory size: %d bytes\n", memory_size);

    // Close the log file and terminate the process
    // fclose(log_file);
    exit(0);
}
