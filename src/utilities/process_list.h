#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include "utilities.h"

#define MAX_LIST_LENGTH 20

typedef enum Process_State {
    WITH_MEMORY_ACCESS, 
    RUNNING, 
    BLOCKED, 
    NOT_DEFINED 
}Process_State;

typedef enum ListNode_State {
    EMPTY,
    OCCUPIED
}ListNode_State;

typedef struct Process_List {
    int pid;
    Process_State proc_state;
    ListNode_State state;
}Process_List;

void initProcessListByDefault(Process_List* list);

int addProcessToList(Process_List* list, int pid, Process_State proc_state);

void changeProcState(Process_List* list, Process_State newState, int index);

void removeProcessFromList(Process_List* list, int index);

#endif