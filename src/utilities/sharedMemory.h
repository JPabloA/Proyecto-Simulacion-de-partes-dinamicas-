#include <stdlib.h>

#ifndef SHARED_MEMORY_FILE
#define SHARED_MEMORY_FILE

int createSharedMemorySegment(const char* filepath, int id, size_t size);

int getSharedMemorySegment(const char* filepath, int id);

void releaseSharedMemorySegment(const char* filepath, int id);

void* attachSharedMemorySegment(int shmid);

void detachSharedMemorySegment(void* ptr);

#endif