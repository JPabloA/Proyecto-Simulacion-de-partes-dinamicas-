#include <stdio.h>

#define FILENAME "./sharedFile"
#define SHARED_INFO "./sharedInfo"
#define SNAME "/MemSemaphore"

enum State
{
    InUse,
    Available
};

enum Algorithm
{
    BestFit,
    FirstFit,
    WorstFit
};

typedef struct
{
    enum State state;
    int pid;
} Line;

typedef struct {
    int pid;
    int time;
    int lines;
} ThreadProcess;

typedef struct {
    int num_lines;
} SharedInformation;
