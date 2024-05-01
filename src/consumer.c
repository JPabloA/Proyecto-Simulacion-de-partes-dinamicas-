#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>

#define SHM_NAME "/shm_example"
#define SEM_NAME "/sem_example"

int main() {
    // Abrir un semáforo con nombre existente
    sem_t *sem = sem_open(SEM_NAME, 0);

    // Abrir el bloque de memoria compartida existente
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0644);
    int *shared_val = (int *)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Decrementar el valor en la memoria compartida
    for (int i = 0; i < 20; i++) {
        sem_wait(sem); // Bloquear el semáforo
        (*shared_val)--;
        printf("Consumidor decrementó el valor a %d\n", *shared_val);
        sem_post(sem); // Desbloquear el semáforo
        sleep(1); // Pausa para mostrar un cambio
    }

    // Cerrar recursos
    munmap(shared_val, sizeof(int));
    sem_close(sem);

    return 0;
}
