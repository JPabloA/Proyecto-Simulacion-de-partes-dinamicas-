#include "process_list.h"

void initProcessListByDefault(Process_List* list) {
    for (int i = 0; i < MAX_LIST_LENGTH; i++) {
        list[i].proc = NULL;
        list[i].proc_state = NOT_DEFINED;
        list[i].state = EMPTY;
    }
}

void addProcessToList(Process_List* list, ThreadProcess* proc, Process_State proc_state) {
    for (int i = 0; i < MAX_LIST_LENGTH; i++) {
        if (list[i].state == EMPTY) {
            list[i].proc = proc;
            proc->listIndex = i;
            list[i].proc_state = proc_state;
            list[i].state = OCCUPIED;
            printf("The current state of element in %d is Occupied", i);
            break;
        }
    }
}

void changeProcState(Process_List* list, Process_State newState, int index){
    if (index < MAX_LIST_LENGTH){
        list[index].proc_state = newState;
    }
}

void removeProcessFromList(Process_List* list, int index) {
    if (index >= MAX_LIST_LENGTH || list[index].state == EMPTY)
        return;

    list[index].proc = NULL;
    list[index].proc_state = NOT_DEFINED;
    list[index].state = EMPTY;
}
