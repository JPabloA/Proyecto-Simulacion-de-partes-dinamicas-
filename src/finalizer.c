#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define FILENAME "./sharedFile"

// COMANDO IMPORTANTE: ipcs -m OR ipcs -m -i <ID del segment>

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Argumentos incorrectos!\nUso: %s <size_in_bytes>\n", argv[0]);
        return 1;
    }

    char *endptr;
    long int size = strtol(argv[1], &endptr, 10);

    if (size <= 0 || *endptr != '\0') {
        fprintf(stderr, "Invalid arguments types\n");
        return 1;
    }

    key_t key = ftok(FILENAME, 's');
    int shmid;

    // Look for the shared segment
    shmid = shmget(key, size, 0644 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Delete shared memory
    int status = shmctl(shmid, IPC_RMID, NULL);
    if (status < 0) {
        perror("shmctl");
        exit(1);
    }

    printf("Shared memory segment free and released\n");

    return 0;
}
