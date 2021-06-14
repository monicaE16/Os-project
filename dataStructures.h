#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <string.h>

/////////////////////////////////////////// INITIALIZE THE REMAINING TIME TO BE EQUAL TO THE RUNNING TIME AT THE CREATION OF THE PROCESS

typedef short bool;
#define true 1
#define false 0

typedef struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
} processData;

typedef struct pcb
{
    struct processData process;
    int state; // 0 = started, 1 = resumed, 2 = stopped, 3 = finished.
    int startTime;
    int remainingTime;
    int waitingTime;
    int pid;
} pcb;

// this node should hold a process and its data
typedef struct node
{

    pcb *data;
    struct node *next;
} node;

typedef struct queue
{
    node *head;
    node *tail;

} queue;

union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}

void initQueue(queue *q)
{
    q->head = NULL;
    q->tail = NULL;
}

void print(queue *q) //Print the data of Queue
{

    node *temp = q->head;
    while (temp != NULL)
    {
        printf("\nProcess Id = %d\tPriority = %d \n", temp->data->process.id, temp->data->process.priority);
        temp = temp->next;
    }
}

void printList(queue *q)
{
    node *current = q->head;
    while (current != NULL)
    {
        printf("[%d,%d] -> ",current->data->process.id,current->data->state);
        current = current->next;
    }
    printf("NULL\n");
}

// Function to Create A New Node
node *newNode(pcb *pd)
{
    node *temp = (node *)malloc(sizeof(node));
    temp->data = pd;
    temp->next = NULL;

    return temp;
}

// // Return the value at head
node* peek(queue *q)
{
    return q->head;
}

// Removes the element with the
// highest priority form the list
node *dequeue(queue *q)
{
    node *temp = q->head;
    (q->head) = (q->head)->next;
    // free(temp);
    return temp;
}

// Function to push according to priority
void enqueue_priority(queue *q, node *pd)
{
    node *start = (q->head);
    // node *start = (head);
    // Create new Node
    node *temp = pd;

    if ((q->head == NULL))
    {
        temp->next = NULL;
        // f = temp;
        q->head = temp;
        return;
    }
    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if ((q->head)->data->process.priority > pd->data->process.priority)
    {

        // Insert New Node before head
        temp->next = q->head;
        // (head) = temp;
        q->head = temp;
    }
    else
    {

        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
               start->next->data->process.priority < pd->data->process.priority)
        {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

///IT SHOULD BE ENQUEUED BY THE RUNNING TIME & WE SHOULD CONSIDER THAT THE REMAINNING TIME AND
/// THE RUNNING TIME ARE THE SAME VARIABLE SO
// Function to push according to priority
void enqueue_rem_time(queue *q, node *pd)
{
    node *start = (q->head);
    // node *start = (head);
    // Create new Node
    node *temp = pd;

    if ((q->head == NULL))
    {
        temp->next = NULL;
        // f = temp;
        q->head = temp;
        return;
    }
    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if ((q->head->data->state == 3 || ((q->head)->data->remainingTime > pd->data->remainingTime)) && pd->data->state != 3)
    {

        // Insert New Node before head
        temp->next = q->head;
        // (head) = temp;
        q->head = temp;
    }
    else
    {

        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL && ((start->next->data->remainingTime < pd->data->remainingTime) || pd->data->state == 3))
        {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

// Function to enqueue normally
void enqueue(queue *q, node *pd)
{
    node *start = (q->head);
    // node *start = (head);
    // Create new Node
    node *temp = pd;

    if ((q->head == NULL))
    {
        temp->next = NULL;
        // f = temp;
        q->head = temp;
        return;
    }

    while (start->next != NULL)
    {
        start = start->next;
    }
    start->next = temp;
    temp->next = NULL;
    return;
}

// Function to check is list is empty
bool isEmpty(queue q)
{
    return (q.head) == NULL;
}
void insertQueue(queue *readyQueue, pcb *current_process_b, char *algo, char* quantum)
{

    node *current_process_node = newNode(current_process_b);
    if (atoi(algo) == 1)
    { /// FCFS
        enqueue(readyQueue, current_process_node);
    }
    else if (atoi(algo) == 2)
    { //SJF
        enqueue_rem_time(readyQueue, current_process_node);
    }
    else if (atoi(algo) == 3)
    { //HPF
        printf("HELLO BEFORE NO 3!");
        enqueue_priority(readyQueue, current_process_node);
        printf("hELLO AFTER NO3!");
    }
    else if (atoi(algo) == 4)
    { //SRTN
        enqueue_rem_time(readyQueue, current_process_node);
    }
    // print(readyQueue);
       else if(atoi(algo) == 5){//RR
       enqueue(readyQueue, current_process_node);
       }
}

// void FCFS(queue *readyQueue, pcb *current_running_process, int* sem1, int* sem2, int* sem3)
// {
//     if (!isEmpty(*readyQueue) || current_running_process != NULL)
//     {
//         if (current_running_process == NULL)
//         {
//             down(*sem2);
//             node *nn = dequeue(readyQueue);
//             current_running_process = nn->data;
//             up(*sem1);
//         }
//         char *number_temp = malloc(sizeof(char));
//         int remaining_time = current_running_process->remainingTime--;
//         printf("FROM THE PARENT: %d\n", remaining_time);
//         sprintf(number_temp, "%d", remaining_time);
//         strcpy((char *)shmaddr, number_temp);
//         if (current_running_process->remainingTime <= 0)
//         {
//             current_running_process = NULL;
//             up(*sem2);
//         }
//         up(*sem3);
//     }
// }