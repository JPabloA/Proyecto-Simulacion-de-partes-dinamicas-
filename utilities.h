#include <stdio.h>

enum State {
    InUse,
    Available
};

typedef struct {
    enum State state; 
    int pid;      
    int time;   
} Line;