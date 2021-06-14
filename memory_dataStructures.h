
// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/sem.h>
// #include <string.h>
#include "dataStructures.h"

/////////////////////////////////////////// INITIALIZE THE REMAINING TIME TO BE EQUAL TO THE RUNNING TIME AT THE CREATION OF THE PROCESS

// typedef short bool;
// #define true 1
// #define false 0
// #define totalMemSize 1024

// typedef struct processData
// {
//     int arrivaltime;
//     int priority;
//     int runningtime;
//     int memorysize;
//     int id;
// } processData;

// typedef struct pcb
// {
//     struct processData process;
//     int state; // 0 = started, 1 = resumed, 2 = stopped, 3 = finished.
//     int startTime;
//     int remainingTime;
//     int waitingTime;
//     int pid;
// } pcb;

// this memNode should hold a process and its data
typedef struct memNode
{
    pcb *data;
    int startLocation;
    int size;
    bool isFree;
    struct memNode *next;
} memNode;

typedef struct linkedList
{
    memNode *head;
    memNode *tail;

} linkedList;


linkedList initLinkedList()
{
    linkedList l;
    memNode* baseNode;
    baseNode->isFree = true;
    baseNode->size = totalMemSize;
    baseNode->startLocation = 0;
    baseNode->data = NULL;
    l.head = baseNode;
    baseNode->next = NULL;
    return l;
    // q->tail = baseNode;
}


void printList(linkedList *q)
{
    memNode *current = q->head;
    while (current != NULL)
    {
        // printf("[%d,%d] -> ", current->data->process.id, current->data->state);
        current = current->next;
    }
    printf("NULL\n");
}

// Function to Create A New memNode
memNode *newmemNode(pcb *pd)
{
    memNode *temp = (memNode *)malloc(sizeof(memNode));
    temp->data = pd;
    temp->next = NULL;

    return temp;
}

// bool enqueueIn(linkedList *l, pcb* p){
//     memNode *start = (l->head);
//     pcb *temp = p;

//     if (start->size>=p->process.memorysize && start->isFree){
//         start->size = start->size - p->process.memorysize;
//         temp->next = start;
//         l->head = temp;
//     }
//     // while()

//     while(start->next->size<p->next->size && !start->isFree && start-> next != NULL){
//         start  = start->next;
//     }
//     if (start->next->size>=p->size && !start->next->isFree ){
//         start->next->size = start->next->size - p->size;
//         temp->next = start->next;
//         start->next = temp;
//         return true;
//     }
//     return false;
//     // if(start->next == NULL){
//     //     return false;
//     // }

// }


// // Function to check is list is empty
// bool isEmpty(LinkedList q)
// {
//     return (q.head) == NULL;
// }

bool checkAvailableMem(pcb *current_process, char *memAlgo, void *memory)
{
    bool checker = false;
    if (atoi(memAlgo) == 1)
    { //First Fit
    }
    else if (atoi(memAlgo) == 2)
    { // Next Fit
    }
    else if (atoi(memAlgo) == 3)
    { // Best Fit
    }
    else if (atoi(memAlgo) == 4)
    {// Buddy System allocation
    }

    return checker;
}


// NSHOOF EL WAITING LinkedList
void deallocate(int processID, char *memAlgo, void *memory ){
    if (atoi(memAlgo) == 4)
    {// Buddy System allocation

    }
    else{

    }
}
