#include <stdlib.h>
#include <stdio.h>

#include <semaphore.h>

#ifndef SHARED_SEMAPHORE
#define SHARED_SEMAPHORE

sem_t* OpenSemaphore(const char* semaphore_name, int flag);

sem_t* GetSemaphore(const char* semaphore_name);

void CloseSemaphore(sem_t* semaphore);

void UnlinkSemaphore(const char* semaphore_name);

#endif