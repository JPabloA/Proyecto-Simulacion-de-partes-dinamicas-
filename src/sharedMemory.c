#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int createSharedMemorySegment(const char* filepath, int id, size_t size) {
    key_t key = ftok(filepath, id);

    int shmid = shmget(key, size, IPC_CREAT | 0666);
    if (shmid < 0) {
        printf("ERROR: Failed creating memory segment witd ID: %d and key: %x\n", shmid, key);
        return -1;
    }

    printf("Shared memory segment created with ID: %d and key: %x\n", shmid, key);
    return shmid;
}

int getSharedMemorySegment(const char* filepath, int id) {
    key_t key = ftok(filepath, id);

    int shmid = shmget(key, 0, 0);
    if (shmid < 0) {
        printf("ERROR: Failed getting memory segment witd ID: %d and key: %x\n", shmid, key);
        return -1;
    }

    printf("Shared memory segment found with ID: %d and key: %x\n", shmid, key);
    return shmid;
}

void releaseSharedMemorySegment(const char* filepath, int id) {

    int shmid = getSharedMemorySegment(filepath, id);

    printf("Released ID: %d\n", shmid);

    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        printf("ERROR: Failed releasing shared memory segment\n");
        return;
    }
    printf("Shared memory segment released\n");
}

void* attachSharedMemorySegment(int shmid) {

    void* result = shmat(shmid, NULL, 0);

    if (result == (void*) -1) {
        printf("ERROR: Failed attaching shared memory segment - shmat\n");
        return NULL;
    }

    return result;
}

void detachSharedMemorySegment(void* ptr) {

    if (shmdt(ptr) < 0) {
        printf("ERROR: Failed detaching shared memory segment for attach ptr - shmdt\n");
        return ;
    }

    printf("Shared memory segment detach for attach ptr\n");
}