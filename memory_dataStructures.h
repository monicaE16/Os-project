#include "dataStructures.h"

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
    memNode *nextFit;
    memNode *tail;

} linkedList;

linkedList *initLinkedList()
{
    linkedList *l = (linkedList *)malloc(sizeof(linkedList));
    memNode *baseNode = (memNode *)malloc(sizeof(memNode));

    baseNode->isFree = true;
    baseNode->size = totalMemSize;
    baseNode->startLocation = 0;
    baseNode->data = NULL;
    baseNode->position = 1;
    baseNode->level = 0;
    baseNode->next = NULL;
    baseNode->myCount = 1;

    l->head = baseNode;
    l->nextFit = l->head;
    return l;
    // q->tail = baseNode;
}

void printLinkedList(linkedList *q)
{
    memNode *current = q->head;
    while (current != NULL)
    {
        printf("[%d,%d] -> ", current->size, current->isFree);
        current = current->next;
    }
    printf("NULL\n");
}

// Function to Create A New memNode
memNode *newmemNode(pcb *pd)
{
    memNode *temp = (memNode *)malloc(sizeof(memNode));
    temp->data = pd;
    temp->size = pd->process.memorysize;

    temp->next = NULL;
    return temp;
}

bool insertNextFit(linkedList *l, pcb *p)
{
    memNode *tempStart = (l->nextFit); //this pointer prevents me from rotating in an infinite loop
    pcb *temp = p;

    if (l->nextFit->size >= p->process.memorysize && l->nextFit->isFree)
    {
        memNode *newNode = newmemNode(p);
        printf("%d\n", newNode->size);
        l->nextFit->size = l->nextFit->size - newNode->size;
        newNode->startLocation = 0;
        l->nextFit->startLocation = newNode->size;
        newNode->next = l->nextFit;
        newNode->isFree = false;
        if (l->nextFit->startLocation==0)
        {
            l->head = newNode;
        }
        newNode = NULL;
        return true;
    }

    int local_Location = l->nextFit->startLocation + l->nextFit->size;
    while (l->nextFit != tempStart)
    {
        while (l->nextFit->next != NULL)
        {
            if ((l->nextFit->next->size >= p->process.memorysize) && l->nextFit->next->isFree)
            {
                memNode *newNode = newmemNode(p);

                l->nextFit->next->size = l->nextFit->next->size - p->process.memorysize;
                l->nextFit->next->startLocation = local_Location + p->process.memorysize;
                if (l->nextFit->next->size == 0)
                { ////////

   
                    newNode->startLocation = l->nextFit->next->startLocation - p->process.memorysize;
                    newNode->isFree = false;

                    l->nextFit->next = newNode;
                    newNode->next = NULL;

                    
                    l->nextFit=newNode;
                    return true;
                }
                newNode->next = l->nextFit->next;
                newNode->startLocation = local_Location;
                l->nextFit->next = newNode; //
                newNode->isFree = false;
                 
                l->nextFit=newNode; 
                return true;
            }
            else
            {
                local_Location += l->nextFit->next->size; //should be start next
                l->nextFit = l->nextFit->next;
            }
        }
        //wrap around
        l->nextFit = l->head;
    }
    return false;
}

bool insertFirstFit(linkedList *l, pcb *p)
{
    // printf("HELLO1!\n");
    memNode *start = (l->head);
    pcb *temp = p;

    if (start->size >= p->process.memorysize && start->isFree)
    {
        // printf("HELLO2!\n");
        memNode *newNode = newmemNode(p);
        printf("%d\n", newNode->size);
        start->size = start->size - newNode->size;
        newNode->startLocation = 0;
        start->startLocation = newNode->size;
        newNode->next = start;
        newNode->isFree = false;
        l->head = newNode;
        newNode = NULL;
        return true;
    }
    

    int local_Location = 0 + start->size;

    while (start->next != NULL)
    {
        if ((start->next->size >= p->process.memorysize) && start->next->isFree)
        {
            memNode *newNode = newmemNode(p);

            start->next->size = start->next->size - p->process.memorysize;
            start->next->startLocation = local_Location + p->process.memorysize;
            if (start->next->size == 0)
            {

                start->next->size = p->process.memorysize;
                start->next->startLocation -= p->process.memorysize;
                start->next->isFree = false;
                newNode->next = NULL;

                // start->next->size = p->process.memorysize;
                // start->next->startLocation-= p->process.memorysize;
                
                return true;
            }
            newNode->next = start->next;
            newNode->startLocation = local_Location;
            start->next = newNode;
            newNode->isFree = false;

            return true;
        }
        else
        {
            local_Location += start->next->size; //should be start next
            start = start->next;
        }
    }
    return false;
}

// To dequeue using the first fit algorithm
void dequeueFF(linkedList *list, int toBeRemovedID)
{
    // If the memory is empty
    if (list->head->size == 1024 && list->head->isFree)
    {
        return;
    }

    memNode *tempNode = list->head;
    if (tempNode->data->process.id == toBeRemovedID)
    {
        if (tempNode->next->isFree && tempNode->next != NULL)
        {
            tempNode->next->size += tempNode->size;
            list->head = tempNode->next;
            free(tempNode);
            return;
        }
        else
        {
            tempNode->isFree = true;
            // tempNode->data = NULL;
            return;
        }
    }
    while (tempNode->next != NULL)
    {
        if (tempNode->next->data->process.id == toBeRemovedID)
        {
            if (tempNode->next->next == NULL)
            {
                if (tempNode->isFree)
                {
                    tempNode->size += tempNode->next->size;
                    // tempNode->next->isFree = true;
                    tempNode->next = NULL;
                    return;
                }
                else
                {
                    tempNode->next->isFree = true;
                    return;
                }
            }
            if (tempNode->next->next != NULL && tempNode->next->next->isFree)
            {
                tempNode->next->next->size += tempNode->next->size;
                printf("\n%d\n", tempNode->next->next->size);
                memNode *newNode = tempNode->next;
                if (tempNode->isFree)
                {
                    tempNode->size += tempNode->next->next->size;
                    tempNode->next = tempNode->next->next->next;
                    free(newNode->next);
                    free(newNode);
                    return;
                }

                tempNode->next = tempNode->next->next;

                free(newNode);
                return;
            }
            else if (tempNode->isFree)
            {
                tempNode->size += tempNode->next->size;
                memNode *newNode = tempNode->next;
                tempNode->next = tempNode->next->next;
                free(newNode);
                return;
            }
            else
            {
                tempNode->next->isFree = true;
                // tempNode->data = NULL;
                return;
            }
        }
        else
        {
            tempNode = tempNode->next;
        }
    }
    if (tempNode->data->process.id == toBeRemovedID)
    {
        tempNode->next->isFree = true;
        // tempNode->data = NULL;
        return;
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
        if (list->head->size / 2 >= p->process.memorysize)
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
            int adjustCount;
            if (adjuster != NULL && !adjuster->isFree)
                adjustCount = prev->myCount;
            while (adjuster != NULL && !adjuster->isFree)
            {
                adjustCount++;
                adjuster->myCount = adjustCount;
                adjuster = adjuster->next;
            }
            prev = list->head;
            current = list->head->next;
            continue;
        }
        prev = prev->next;
        current = current->next;
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

    if (!list->head->isFree && list->head->data->process.id == processID)
    {
        pcb *p = list->head->data;
        list->head->isFree = true;
        list->head->data = NULL;
        if (list->head->next != NULL && list->head->next->isFree)
        {
            memNode *afterHead = list->head->next;
            list->head->next = afterHead->next;
            list->head->level--;
            list->head->size *= 2;
            adjustMergeBSA(list);
            // free(afterHead);
        }
        // free(p);
        return;
    }
    memNode *prev = list->head;
    memNode *current = list->head->next;
    bool found = false;
    while (current != NULL)
    {
        if (!current->isFree && current->data->process.id == processID)
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
        if (current->position == 1)
        {
            if (current->next != NULL && current->level == current->next->level && current->next->isFree)
            {
                current->size *= 2;
                memNode *toRemove = current->next;
                current->level--;
                current->next = toRemove->next;
                memNode *adjuster = current->next;
                int adjustCount;
                if (adjuster != NULL && !adjuster->isFree)
                    adjustCount = current->myCount;
                while (adjuster != NULL && !adjuster->isFree)
                {
                    adjustCount++;
                    adjuster->myCount = adjustCount;
                    adjuster = adjuster->next;
                }
                // free(toRemove);
            }
        }
        else if (current->position == 2)
        {
            if (prev->isFree && prev->level == current->level)
            {
                prev->size *= 2;
                memNode *toRemove = current;
                prev->level--;
                prev->next = toRemove->next;
                memNode *adjuster = prev->next;
                int adjustCount;
                if (adjuster != NULL && !adjuster->isFree)
                    adjustCount = prev->myCount;
                while (adjuster != NULL && !adjuster->isFree)
                {
                    adjustCount++;
                    adjuster->myCount = adjustCount;
                    adjuster = adjuster->next;
                }
                // free(toRemove);
            }
        }
        adjustMergeBSA(list);
    }
}

bool checkAvailableMem(pcb *current_process, char *memAlgo, linkedList *memory)
{
    bool checker = false;
    if (atoi(memAlgo) == 1)
    { //First Fit
        return insertFirstFit(memory, current_process);
    }
    else if (atoi(memAlgo) == 2)
    {
        return insertNextFit(memory, current_process);
    }
    else if (atoi(memAlgo) == 3)
    { // Best Fit
    }
    else if (atoi(memAlgo) == 4)
    { // Buddy System allocation
        return insertBSA(memory, current_process);
    }

    return checker;
}

// // NSHOOF EL WAITING LinkedList
void deallocate(int processID, char *memAlgo, linkedList *memory)
{
    if (atoi(memAlgo) == 4)
    { // Buddy System allocation
        deallocateBSA(memory, processID);
    }
    else
    {
        dequeueFF(memory, processID);
    }
}

void allocateTheWaitingList(queue *waitingQueue, linkedList *mem, queue *readyQueue, char *algo, char *quantum, char *memAlgo)
{

    node *current_node = waitingQueue->head;
    if (waitingQueue->head == NULL)
    {
        return;
    }

    if (current_node != NULL)
    {
        bool canAllocate = checkAvailableMem(current_node->data, memAlgo, mem);
        printf("\n %d  %d\n", canAllocate, current_node->data->process.memorysize);
        if (canAllocate)
        {
            waitingQueue->head = current_node->next;
            int pid_process = fork();
            if (pid_process == -1)
            {
                perror("error in fork");
            }
            else if (pid_process == 0) // If the process is a child
            {
                char *const argv_scheduler[] = {"./process.out", NULL};
                if (execv(argv_scheduler[0], argv_scheduler) == -1)
                {
                    printf("error in executing the file");
                }
            }

            current_node->data->pid = pid_process;
            // Insert the new process into a queue according to the algorithm recieved
            insertQueue(readyQueue, current_node->data, algo, quantum);
            kill(pid_process, SIGSTOP);
            printf("W HNA KMAN\n");
            // return;
        }
    }
            printf("ANA HENA DELW2ty\n");

    while (current_node->next != NULL)
    {
        bool canAllocate = checkAvailableMem(current_node->next->data, memAlgo, mem);
        printf("\n %d\n", canAllocate);
        if (canAllocate)
        {
            node *tempNode = current_node->next;
            current_node->next = current_node->next->next;

            // Forking a new process
            int pid_process = fork();
            if (pid_process == -1)
            {
                perror("error in fork");
            }
            else if (pid_process == 0) // If the process is a child
            {
                char *const argv_scheduler[] = {"./process.out", NULL};
                if (execv(argv_scheduler[0], argv_scheduler) == -1)
                {
                    printf("error in executing the file");
                }
            }

            current_node->next->data->pid = pid_process;
            // Insert the new process into a queue according to the algorithm recieved
            insertQueue(readyQueue, current_node->next->data, algo, quantum);
            kill(pid_process, SIGSTOP);
            // return;
        }
        current_node = current_node->next;
    }
}
