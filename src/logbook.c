#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>

// Estructura para la información de la bitácora
typedef struct {
    int pid;
    char accion[50];
    char tipo[20];
    char hora[20];
    int lineas;
} BitacoraEntry;

// Función para obtener la hora actual como cadena
void get_current_time(char* buffer, size_t size) {
    time_t raw_time = time(NULL);
    struct tm* time_info = localtime(&raw_time);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", time_info);
}

// Función para escribir en la bitácora
void write_to_bitacora(const char* filename, sem_t* semaphore, BitacoraEntry* entry) {
    // Esperar a obtener el semáforo
    sem_wait(semaphore);

    // Abrir el archivo de bitácora para agregar datos
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error al abrir la bitácora");
        sem_post(semaphore);  // Liberar el semáforo si hay un error
        exit(1);
    }

    // Escribir la entrada en la bitácora
    fprintf(file, "%d | %s | %s | %s | %d\n",
            entry->pid, entry->accion, entry->tipo, entry->hora, entry->lineas);

    // Cerrar el archivo y liberar el semáforo
    fclose(file);
    sem_post(semaphore);
}



int main() {
    // Crear o abrir un semáforo para sincronizar el acceso a la bitácora
    sem_t* bitacora_semaphore = sem_open("/bitacora_semaphore", O_CREAT, 0644, 1);
    if (bitacora_semaphore == SEM_FAILED) {
        perror("Error al crear el semáforo");
        exit(1);
    }

    // Configurar la entrada para la bitácora
    BitacoraEntry entry;
    entry.pid = getpid(); // Obtener el PID del proceso actual
    strcpy(entry.accion, "Asignación de memoria");
    strcpy(entry.tipo, "Asignación");
    get_current_time(entry.hora, sizeof(entry.hora));  // Obtener la hora actual
    entry.lineas = 5;  // Número de líneas asignadas

    // Escribir en la bitácora
    write_to_bitacora("bitacora.txt", bitacora_semaphore, &entry);

    // Cerrar el semáforo
    sem_close(bitacora_semaphore);

    return 0;
}
