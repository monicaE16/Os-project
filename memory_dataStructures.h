
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
    int position;
    int level;
    int myCount;
} memNode;

typedef struct linkedList
{
    memNode *head;
    memNode *tail;

} linkedList;

linkedList *initLinkedList()
{
    linkedList *l = (linkedList *)malloc(sizeof(linkedList));
    memNode *baseNode;
    baseNode->isFree = true;
    baseNode->size = totalMemSize;
    baseNode->startLocation = 0;
    baseNode->data = NULL;
    baseNode->position = 1;
    baseNode->level = 0;
    baseNode->next = NULL;
    baseNode->myCount = 1;
    l->head = baseNode;
    return l;
    // q->tail = baseNode;
}

void printListMem(linkedList *q)
{
    memNode *current = q->head;
    while (current != NULL)
    {
        printf("[%d,%d,%d,%d] -> ", current->size, current->isFree, current->level, current->position);
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
    { // Buddy System allocation
    }

    return checker;
}

// NSHOOF EL WAITING LinkedList
void deallocate(int processID, char *memAlgo, void *memory)
{
    if (atoi(memAlgo) == 4)
    { // Buddy System allocation
    }
    else
    {
    }
}

memNode *newmemNodeBSA(int memorySize, int startLocation)
{
    memNode *temp = (memNode *)malloc(sizeof(memNode));
    temp->size = memorySize;
    temp->isFree = true;
    temp->next = NULL;
    temp->startLocation = startLocation;
    temp->data = NULL;
    return temp;
}

bool insertBSA(linkedList *list, pcb *p)
{
    if (!list->head->isFree && list->head->next == NULL)
        return false;

    if (list->head->next == NULL && list->head->isFree)
    {
        if (list->head->size / 2 == p->process.memorysize)
        {
            list->head->data = p;
            list->head->isFree = false;
            return true;
        }
        else if (list->head->size / 2 >= p->process.memorysize)
        {
            memNode *afterHead = newmemNodeBSA(list->head->size / 2, list->head->size / 2);
            afterHead->position = 2;
            afterHead->myCount = 2;
            list->head->level++;
            afterHead->level = list->head->level;
            list->head->next = afterHead;
            list->head->size /= 2;
        }
    }

    memNode *current = list->head;
    memNode *blockToUse = list->head;
    bool found = false;
    while (current != NULL)
    {
        if (current->isFree && current->size >= p->process.memorysize)
        {
            if (found)
            {
                if (blockToUse->size > current->size)
                {
                    blockToUse = current;
                    found = true;
                }
            }
            else
            {
                blockToUse = current;
                found = true;
            }
        }
        current = current->next;
    }
    if (found || (blockToUse->isFree && blockToUse->size >= p->process.memorysize))
    {
        while (blockToUse->size / 2 >= p->process.memorysize)
        {
            memNode *myNewNode = newmemNodeBSA(blockToUse->size / 2, blockToUse->startLocation + blockToUse->size / 2);
            myNewNode->myCount = blockToUse->myCount + 1;
            blockToUse->size /= 2;
            blockToUse->level++;
            myNewNode->level = blockToUse->level;
            if (blockToUse->position == 2)
                blockToUse->position = 1;
            myNewNode->position = 2;
            myNewNode->next = blockToUse->next;
            blockToUse->next = myNewNode;

            memNode *adjuster = myNewNode->next;
            int adjustCount = myNewNode->myCount;
            while (adjuster != NULL)
            {
                adjustCount++;
                adjuster->myCount = adjustCount;
                adjuster = adjuster->next;
            }
        }
        blockToUse->isFree = false;
        blockToUse->data = p;
        return true;
    }
    return false;
}

void adjustMergeBSA(linkedList *list)
{
    if (list->head == NULL)
        return;

    if (list->head->next == NULL)
    {
        list->head->myCount = 1;
        list->head->position = 1;
        list->head->level = 0;
        return;
    }

    memNode *prev = list->head;
    memNode *current = list->head->next;
    while (prev != NULL && current != NULL)
    {
        if (prev->isFree && current->isFree && current->level == prev->level)
        {
            prev->size *= 2;
            memNode *toRemove = current;
            prev->level--;
            prev->next = toRemove->next;
            memNode *adjuster = prev->next;
            int adjustCount = prev->myCount;
            while (adjuster != NULL)
            {
                adjustCount++;
                adjuster->myCount = adjustCount;
                adjuster = adjuster->next;
            }
            prev = list->head;
            current = list->head->next;
        }
    }
    
    current = list->head;
    int nodePosition = 1;
    while (current != NULL)
    {
        if (nodePosition > 3)
            nodePosition = 1;
        current->position = nodePosition;
        nodePosition++;
        current = current->next;
    }
}

void deallocateBSA(linkedList *list, int processID)
{
    if (list->head->isFree && list->head->next == NULL)
        return;

    if (list->head->data->pid == processID)
    {
        pcb *p = list->head->data;
        list->head->isFree = true;
        if (list->head->next != NULL && list->head->next->isFree)
        {
            memNode *afterHead = list->head->next;
            list->head->next = afterHead->next;
            list->head->level--;
            list->head->size *= 2;
            free(afterHead);
        }
        free(p);
        return;
    }

    memNode *prev = list->head;
    memNode *current = list->head->next;
    bool found = false;
    while (current != NULL)
    {
        if (current->data->pid == processID)
        {
            found = true;
            break;
        }
        prev = prev->next;
        current = current->next;
    }

    if (found)
    {
        current->isFree = true;
        pcb *p = current->data;
        current->data = NULL;
        bool canMerge = false;
        if (current->position == 1)
        {
            if (current->next != NULL && current->level == current->next->level && current->next->isFree)
            {
                canMerge = true;
                current->size *= 2;
                memNode *toRemove = current->next;
                current->level--;
                current->next = toRemove->next;
                memNode *adjuster = current->next;
                int adjustCount = current->myCount;
                while (adjuster != NULL)
                {
                    adjustCount++;
                    adjuster->myCount = adjustCount;
                    adjuster = adjuster->next;
                }
                free(toRemove);
            }
        }
        else if (current->position == 2)
        {
            if (prev->isFree && prev->level == current->level)
            {
                canMerge = true;
                prev->size *= 2;
                memNode *toRemove = current;
                prev->level--;
                prev->next = toRemove->next;
                memNode *adjuster = prev->next;
                int adjustCount = prev->myCount;
                while (adjuster != NULL)
                {
                    adjustCount++;
                    adjuster->myCount = adjustCount;
                    adjuster = adjuster->next;
                }
                free(toRemove);
            }
        }
        if (canMerge)
            adjustMergeBSA(list);
    }
}