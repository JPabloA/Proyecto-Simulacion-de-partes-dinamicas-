#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>

#define SHM_NAME "/shm_example"
#define SEM_NAME "/sem_example"

int main() {
    // Crear o abrir un semáforo con nombre
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);

    // Crear o abrir un bloque de memoria compartida
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0644);
    ftruncate(shm_fd, sizeof(int));
    int *shared_val = (int *)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Incrementar el valor en la memoria compartida
    for (int i = 0; i < 20; i++) {
        sem_wait(sem); // Bloquear el semáforo
        (*shared_val)++;
        printf("Productor incrementó el valor a %d\n", *shared_val);
        sem_post(sem); // Desbloquear el semáforo
        sleep(1); // Pausa para mostrar un cambio
    }

    // Cerrar recursos
    munmap(shared_val, sizeof(int));
    sem_close(sem);

    return 0;
}
