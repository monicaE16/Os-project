#include <stdio.h>
#include <stdlib.h>

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

node *f = NULL;
node *r = NULL;

void enqueue(pcb d) //Insert the element and priority in Queue
{
    node *temp;
    node *new_n;
    new_n = (node *)malloc(sizeof(node));
    new_n->data = d;

    if ((f == NULL))
    {
        new_n->next = f;
        f = new_n;
    }
    else
    {
        temp = f;
        while ((temp->next != NULL) && ((temp->next->data.process.priority) <= new_n->data.process.priority))
            temp = temp->next;
        new_n->next = temp->next;
        temp->next = new_n;
    }
}
void print(node *head) //Print the data of Queue
{
    node *temp = head;
    while (temp != NULL)
    {
        printf("\nProcess Id = %d\tPriority = %d ", temp->data.process.id, temp->data.process.priority);
        temp = temp->next;
    }
}
void dequeue() //Deletion of highest priority element from the Queue
{
    node *temp;
    if (f == NULL) //Check for Underflow condition
        printf("\nQueue is Empty");
    else
    {
        temp = f;
        f = f->next;
        printf("\nDeleted element:- %d\t and Its Priority:- %d", temp->data.process.id, temp->data.process.priority);
        free(temp);
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
void pop(node **head)
{
    node *temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

// Function to push according to priority
void push(node **head, pcb pd)
{
    node *start = (*head);

    // Create new Node
    node *temp = newNode(pd);

    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if ((*head)->data.process.priority > pd.process.priority)
    {

        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else
    {

        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
               start->next->data.process.priority < pd.process.priority)
        {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

// Function to check is list is empty
int isEmpty(node **head)
{
    return (*head) == NULL;
}