#include <stdio.h>

#define FILENAME "./sharedFile"
#define SHARED_INFO "./sharedInfo"
#define SNAME "/MemSemaphore"

enum State
{
    InUse,
    Available
};

enum Algoritmo
{
    BestFit,
    FirstFit,
    WorstFit
};

typedef struct
{
    enum State state;
    int pid;
    int time;
} Line;

typedef struct {
    int pid;
    int time;
    int lines;
} ThreadProcess;

typedef struct {
    int num_lines;
} SharedInformation;
