
#include "dataStructures.h"

int main()
{

  printf("hello world");
    processData p1,p2,p3,p4,p5;
    p1.arrivaltime=1;
    p1.id=1;
    p1.priority=5;
    p1.runningtime=4;



    p2.arrivaltime=1;
    p2.id=1;
    p2.priority=3;
    p2.runningtime=4;

    p3.arrivaltime=6;
    p3.id=2;
    p3.priority=6;
    p3.runningtime=4;

    p4.arrivaltime=1;
    p4.id=1;
    p4.priority=2;
    p4.runningtime=4;

node* head=newNode(p1);
push(&head,p2);
push(&head,p3);
push(&head,p4);



print(head);

	
return 0;
}