#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "utilities.h"

int main(int argc, char const *argv[]) {
    struct stat buffer;
    int status;

    printf("Filename: %s\n", FILENAME);

    status = stat(FILENAME, &buffer);

    printf("Status: %d\n", status);
    return 0;
}
