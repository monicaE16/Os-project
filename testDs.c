
#include "dataStructures.h"

int main()
{
  queue q;

  // printf("hello world");
    pcb p1,p2,p3,p4,p5;
    p1.process.arrivaltime=1;
    p1.process.id=3;
    p1.process.priority=5;
    p1.process.runningtime=4;
    p1.remainingTime = 3;
    

    p2.process.arrivaltime=1;
    p2.process.id=1;
    p2.process.priority=3;
    p2.process.runningtime=4;
    p2.remainingTime = 1;

    p3.process.arrivaltime=6;
    p3.process.id=5;
    p3.process.priority=6;
    p3.process.runningtime=4;
    p3.remainingTime = 5;

    p4.process.arrivaltime=1;
    p4.process.id=0;
    p4.process.priority=2;
    p4.process.runningtime=4;
    p4.remainingTime = 0;
    enqueue_rem_time(&q,newNode(p1));
    enqueue_rem_time(&q,newNode(p2));
    enqueue_rem_time(&q,newNode(p3));
    enqueue_rem_time(&q,newNode(p4));
print(q);
printf("\n\n");
    node* test = dequeue(&q);
    // free(test);
printf("\n\n");

// node* head=newNode(p1);
// push(&head,p2);
// push(&head,p3);
// push(&head,p4);
print(q);
enqueue(&q,test);
print(q);


// print(head);

	
return 0;
}