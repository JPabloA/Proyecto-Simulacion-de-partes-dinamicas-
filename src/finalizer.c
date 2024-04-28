#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "utilities.h"
#include "sharedMemory.c"

int main(int argc, char const *argv[]) {

    releaseSharedMemorySegment(FILENAME, 's');
    releaseSharedMemorySegment(SHARED_INFO, 'a');

    return 0;
}
