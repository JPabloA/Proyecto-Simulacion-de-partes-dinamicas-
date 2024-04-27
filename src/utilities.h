#include <stdio.h>

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
enum State
{
    Ocupado,
    Libre
};

typedef struct
{
    enum State state;
    int pid;
    int time;
} Line;