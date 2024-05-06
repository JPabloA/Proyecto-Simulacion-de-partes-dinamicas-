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
    ThreadProcess* proc;
    Process_State proc_state;
    ListNode_State state;
}Process_List;

int addProcessToList(Process_List* list, ThreadProcess* proc, Process_State proc_state);

void removeProcessFromList(Process_List* list, int index);

#endif