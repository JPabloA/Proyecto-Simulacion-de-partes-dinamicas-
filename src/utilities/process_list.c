#include "process_list.h"

int addProcessToList(Process_List* list, ThreadProcess* proc, Process_State proc_state) {
    int index = -1;
    for (int i = 0; i < MAX_LIST_LENGTH; i++) {
        if (list[i].state == EMPTY) {
            list[i].proc = proc;
            list[i].proc_state = proc_state;
            list[i].state = OCCUPIED;

            index = i;
            break;
        }
    }

    return index;
}

void removeProcessFromList(Process_List* list, int index) {
    if (index >= MAX_LIST_LENGTH || list[index].state == EMPTY)
        return;

    list[index].proc = NULL;
    list[index].proc_state = NOT_DEFINED;
    list[index].state = EMPTY;
}
