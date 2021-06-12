#include <stdio.h>
#include <stdlib.h>

/////////////////////////////////////////// INITIALIZE THE REMAINING TIME TO BE EQUAL TO THE RUNNING TIME AT THE CREATION OF THE PROCESS

typedef struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    // int id_next_process;
    // int id_prev_process;
} processData;

typedef struct pcb
{
    struct processData process;
    int state; // 0 = started, 1 = resumed, 2 = stopped, 3 = finished.
    int excutionTime;
    int remainingTime;
    int waitingTime;

} pcb;


// this node should hold a process and its data
typedef struct node
{

    pcb data;
    struct node *next;
} node;

typedef struct queue
{
    node* head;
    node* tail;

}queue;

void print(queue q) //Print the data of Queue
{

    node *temp = q.head;
    while (temp != NULL)
    {
        printf("\nProcess Id = %d\tPriority = %d ", temp->data.process.id, temp->data.process.priority);
        temp = temp->next;
    }
}


// Function to Create A New Node
node *newNode(pcb pd)
{
    node *temp = (node *)malloc(sizeof(node));
    temp->data = pd;
    temp->next = NULL;

    return temp;
}

// Return the value at head
pcb peek(node **head)
{
    return (*head)->data;
}

// Removes the element with the
// highest priority form the list
node* dequeue(queue* q)
{
    node *temp = q->head;
    (q->head) = (q->head)->next;
    // free(temp);
    return temp;
}

// Function to push according to priority
void enqueue_priority(queue* q, node* pd)
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
    if ((q->head)->data.process.priority > pd->data.process.priority)
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
               start->next->data.process.priority < pd->data.process.priority)
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
void enqueue_rem_time(queue* q, node* pd)
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
    if ((q->head)->data.remainingTime > pd->data.remainingTime)
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
               start->next->data.remainingTime < pd->data.remainingTime)
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
void enqueue(queue* q, node* pd)
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

    while(start->next != NULL){
        start = start->next;
    }
    start->next = temp;
    temp->next = NULL;
    return;
}

// Function to check is list is empty
int isEmpty(queue q)
{
    return (q.head) == NULL;
}