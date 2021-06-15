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
//////////////////////////////// Next Fit Algorithm Related Functions ////////////////////////////
bool insertNextFit(linkedList *list, pcb *p, FILE*fp, int currentTime)
{
    //Assuming tail is the nextfit
    if (list->nextFit == NULL)
        return false;

    if (list->head->next == NULL)
    {
        if (list->nextFit->isFree && list->nextFit->size >= p->process.memorysize)
        {
            memNode *createdNode = newmemNode(p);
            createdNode->startLocation = 0;
            createdNode->size = p->process.memorysize;
            createdNode->data = p;
            createdNode->isFree = false;
            list->head->size -= p->process.memorysize;
            fprintf(fp, "At\ttime\t\t%d allocated\t%d\tbytes for process \t%d\tfrom\t%d\tto\t%d\n",
                currentTime+1, createdNode->size, createdNode->data->process.id+1, 
                 createdNode->startLocation,createdNode->startLocation+createdNode->size);
            createdNode->next = list->head;
            list->head = createdNode;
            return true;
        }
        return false;
    }

    memNode *memFitter = list->nextFit;
    bool isWrapped = false;
    while (memFitter != NULL)
    {
        if (isWrapped == true && memFitter == list->nextFit)
            return false;

        if (memFitter->isFree && memFitter->size >= p->process.memorysize)
        {
            memNode *getterPrevFitter = list->head;
            while (getterPrevFitter->next != NULL && getterPrevFitter->next != memFitter)
                getterPrevFitter = getterPrevFitter->next;

            memNode *createdNode = newmemNode(p);
            createdNode->startLocation = memFitter->startLocation;
            createdNode->size = p->process.memorysize;
            createdNode->data = p;
            createdNode->isFree = false;
            createdNode->next = memFitter;
            memFitter->size -= p->process.memorysize;
            getterPrevFitter->next = createdNode;
            if (memFitter->size == 0)
            {
                createdNode->next = memFitter->next;
                list->nextFit = NULL;
            }
            fprintf(fp, "At\ttime\t\t%d allocated\t%d\tbytes for process \t%d\tfrom\t%d\tto\t%d\n",
                currentTime+1, createdNode->size, createdNode->data->process.id+1, 
                 createdNode->startLocation,createdNode->startLocation+createdNode->size);
            
            return true;
        }
        memFitter = memFitter->next;
        if (memFitter == NULL)
        {
            memFitter = list->head;
            isWrapped = true;
        }
    }
    return false;
}

bool insertFirstFit(linkedList *l, pcb *p, FILE* fp, int currentTime)
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

        fprintf(fp, "At\ttime\t\t%d allocated\t%d\tbytes for process \t%d\tfrom\t%d\tto\t%d\n",
                currentTime+1, newNode->size, newNode->data->process.id+1, 
                 newNode->startLocation,newNode->startLocation+newNode->size);
        
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
                start->next = newNode;

                start->next->size = p->process.memorysize;
                start->next->startLocation -= p->process.memorysize;
                start->next->isFree = false;
                newNode->next = NULL;

                fprintf(fp, "At\ttime\t\t%d allocated\t%d\tbytes for process \t%d\tfrom\t%d\tto\t%d\n",
                currentTime+1, newNode->size, newNode->data->process.id+1, 
                 newNode->startLocation,newNode->startLocation+newNode->size);
                

                return true;
            }
            newNode->next = start->next;
            newNode->startLocation = local_Location;
            start->next = newNode;
            newNode->isFree = false;
            
            fprintf(fp, "At\ttime\t\t%d allocated\t%d\tbytes for process \t%d\tfrom\t%d\tto\t%d\n",
                currentTime+1, newNode->size, newNode->data->process.id+1, 
                 newNode->startLocation,newNode->startLocation+newNode->size);

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
void dequeueFF(linkedList *list, int toBeRemovedID, FILE *fp, int currentTime)
{
    // If the memory is empty
    if (list->head->size == 1024 && list->head->isFree)
    {
        return;
    }

    memNode *tempNode = list->head;
    if (tempNode->data->process.id == toBeRemovedID)
    {
            fprintf(fp, "At\ttime\t\t%d   freed\t%d\tbytes for process \t%d\tfrom\t%d\tto\t%d\n",
                currentTime+1, tempNode->size, tempNode->data->process.id+1, 
                 tempNode->startLocation,tempNode->startLocation+tempNode->size);
        if (tempNode->next->isFree && tempNode->next != NULL)
        {
            tempNode->next->size += tempNode->size;
            list->head = tempNode->next;
            if (list->nextFit == NULL)
            {
                list->nextFit = list->head;
            }
            free(tempNode);
            return;
        }
        else
        {
            tempNode->isFree = true;
            if (list->nextFit == NULL)
            {
                list->nextFit = tempNode;
            }
            // tempNode->data = NULL;
            return;
        }
    }
    while (tempNode->next != NULL)
    {
        if (tempNode->next->data->process.id == toBeRemovedID)
        {

                    fprintf(fp, "At\ttime\t\t%d   freed\t%d\tbytes for process \t%d\tfrom\t%d\tto\t%d\n",
                currentTime+1, tempNode->next->size, tempNode->next->data->process.id+1, 
                 tempNode->next->startLocation,tempNode->next->startLocation+tempNode->next->size);
            if (tempNode->next->next == NULL)
            {
                if (tempNode->isFree)
                {
                    tempNode->size += tempNode->next->size;
                    if (list->nextFit == NULL)
                    {
                        list->nextFit = tempNode;
                    }

                    // tempNode->next->isFree = true;
                    tempNode->next = NULL;
                    return;
                }
                else
                {
                    tempNode->next->isFree = true;
                    if (list->nextFit == NULL)
                    {
                        list->nextFit = tempNode->next;
                    }
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
                    newNode->next = NULL;
                    newNode = NULL;
                    if (list->nextFit == NULL)
                    {
                        list->nextFit = tempNode;
                    }
                    free(newNode->next);
                    free(newNode);
                    return;
                }

                tempNode->next = tempNode->next->next;
                if (list->nextFit == NULL)
                {
                    list->nextFit = tempNode->next;
                }

                free(newNode);
                return;
            }
            else if (tempNode->isFree)
            {
                tempNode->size += tempNode->next->size;
                memNode *newNode = tempNode->next;
                tempNode->next = tempNode->next->next;
                if (list->nextFit == NULL)
                {
                    list->nextFit = tempNode;
                }
                free(newNode);
                return;
            }
            else
            {
                tempNode->next->isFree = true;
                if (list->nextFit == NULL)
                {
                    list->nextFit = tempNode->next;
                }
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
        return;
    }
}

bool bestFitEnqueue(linkedList *l, pcb *p) // Memory as linked list & process to add
{
    pcb *temp = p;
    // memNode *bestNode = newMemNode(p);
    memNode *bestNode;
    if (l->head->next == NULL)
    {
        l->head->data = temp;
        bestNode = l->head;
        memNode *newNode = newmemNode(p);
        newNode->size = p->process.memorysize;                   //30
        bestNode->size = bestNode->size - p->process.memorysize; //28
        bestNode->isFree = true;
        newNode->isFree = false;
        // newNode->data->process.id = p->pid;
        bestNode->next = newNode;
        return true;
    }
    else
    {
        int bestDifference = -1;
        int difference;
        memNode *loopNode = l->head;
        while (loopNode != NULL)
        {
            printf("Size of loop node = %d\n", loopNode->size);
            if (loopNode->isFree)
            {
                difference = loopNode->size - p->process.memorysize;
                printf("loopNode->size - p->process.memorysize = %d\n", difference);
                if (difference == 0)
                {
                    bestDifference = difference;
                    bestNode = loopNode;
                    break;
                }
                else if (difference >= 0 && (difference < bestDifference || bestDifference == -1))
                {
                    bestDifference = difference;
                    bestNode = loopNode;
                }
            }
            loopNode = loopNode->next;
        }

        if (bestDifference == -1)
        {
            printf("Couldn't find a node with enough available memory space!\n");
            return false;
        }
        else if (bestDifference == 0)
        { // exact size
            bestNode->size = p->process.memorysize;
            bestNode->isFree = false;
            bestNode->data = p;
            return true;
        }
        else
        { // node size greater than process size = split
            memNode *newNode = newmemNode(p);
            newNode->size = p->process.memorysize;                   //30
            bestNode->size = bestNode->size - p->process.memorysize; //28
            bestNode->isFree = true;
            newNode->isFree = false;
            // newNode->data->process.id = p->pid;
            newNode->next = bestNode->next;
            bestNode->next = newNode;
            return true;
        }
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

void deallocateBSA(linkedList *list, int processID, FILE* fp, int currentTime)
{
    if (list->head->isFree && list->head->next == NULL)
        return;

    if (!list->head->isFree && list->head->data->process.id == processID)
    {
        fprintf(fp, "At\ttime\t\t%d   freed\t%d\tbytes for process \t%d\tfrom\t%d\tto\t%d\n",
                currentTime+1, list->head->size, list->head->data->process.id+1, 
                 list->head->startLocation,list->head->startLocation+list->head->size);
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
        fprintf(fp, "At\ttime\t\t%d   freed\t%d\tbytes for process \t%d\tfrom\t%d\tto\t%d\n",
                currentTime+1, current->size, current->data->process.id+1, 
                 current->startLocation,current->startLocation+current->size);
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

bool checkAvailableMem(pcb *current_process, char *memAlgo, linkedList *memory, FILE* fp, int currentTime)
{
    bool checker = false;
    if (atoi(memAlgo) == 1)
    { //First Fit
        return insertFirstFit(memory, current_process, fp, currentTime);
    }
    else if (atoi(memAlgo) == 2)
    {
        return insertNextFit(memory, current_process, fp, currentTime);
    }
    else if (atoi(memAlgo) == 3)
    { // Best Fit
        return bestFitEnqueue(memory, current_process);
    }
    else if (atoi(memAlgo) == 4)
    { // Buddy System allocation
        return insertBSA(memory, current_process);
    }

    return checker;
}

// // NSHOOF EL WAITING LinkedList
void deallocate(int processID, char *memAlgo, linkedList *memory, FILE *fp, int currentTime)
{
    if (atoi(memAlgo) == 4)
    { // Buddy System allocation
        deallocateBSA(memory, processID, fp, currentTime);
    }
    else
    {
        dequeueFF(memory, processID, fp, currentTime);
    }
}

void allocateTheWaitingList(queue *waitingQueue, linkedList *mem, queue *readyQueue, char *algo, char *quantum, char *memAlgo,FILE* fp, int currentTime)
{

    node *current_node = waitingQueue->head;
    if (waitingQueue->head == NULL)
    {
        return;
    }

    if (current_node != NULL)
    {
        bool canAllocate = checkAvailableMem(current_node->data, memAlgo, mem, fp, currentTime);
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
        bool canAllocate = checkAvailableMem(current_node->next->data, memAlgo, mem, fp, currentTime);
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
