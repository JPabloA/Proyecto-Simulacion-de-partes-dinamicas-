#include "sharedSemaphore.h"

sem_t* OpenSemaphore(const char* semaphore_name, int flag) {
    sem_t* semaphore = sem_open(semaphore_name, flag, 0644, 1);
    if (semaphore == SEM_FAILED) {
        return NULL;
    }

    return semaphore;
}

sem_t* GetSemaphore(const char* semaphore_name) {
    sem_t* semaphore = sem_open(semaphore_name, 0);
    if (semaphore == SEM_FAILED) {
        return NULL;
    }

    return semaphore;
}

void CloseSemaphore(sem_t* semaphore) {
    if (sem_close(semaphore) < 0) {
        printf("ERROR: Failed closing semaphore\n");
        return;
    }
    printf("Semaphore closed succesfully\n");
}

void UnlinkSemaphore(const char* semaphore_name) {
    if (sem_unlink(semaphore_name) < 0) {
        printf("ERROR: Failed unlinking semaphore\n");
        return;
    }
    printf("Semaphore unlinked succesfully\n");
}