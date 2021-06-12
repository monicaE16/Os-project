#include "headers.h"
#include <string.h>

struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    // int id_next_process;
    // int id_prev_process;
};

struct pcb
{
    struct processData process;
    int state; // 0 = started, 1 = resumed, 2 = stopped, 3 = finished.
    int excutionTime;
    int remainingTime;
    int waitingTime;
};

struct msgbuff
{
    long mtype;
    // char mtext[256];
    struct processData mmsg;
};

int main(int argc, char *argv[])
{
    char *algo;
    strcpy(algo,argv[1]);
    printf("ARGC: %d\tARGUMENT SENT: %s\n",argc, algo);

    key_t process_key_id;
    int process_msgq_id;

    process_key_id = ftok("keyfile", 'a'); //create unique key

    printf("ALGO CHOSEN: %s   %s  %s\n", argv[0], argv[1], argv[2]);

    process_msgq_id = msgget(process_key_id, 0666 | IPC_CREAT); //create message queue and return id
    if (process_msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }

    initClk();
    struct msgbuff process;
    printf("Hello frm scheduler!\n");
    int index = 0;
    struct pcb *pData = (struct pcb *)malloc(1 * sizeof(struct pcb));
    while (1)
    {
        // To get time use this function.
        int x = getClk();

        int rec_val = 0;
        while (rec_val != -1)
        {

            // printf("Current Time is %d\n", x);
            rec_val = msgrcv(process_msgq_id, &process, sizeof(process.mmsg), 0, IPC_NOWAIT);

            if (rec_val != -1)
            {
                printf("Message received in server: %d\n", process.mmsg.id);
                struct pcb current_process_b;
                current_process_b.process = process.mmsg;
                current_process_b.state = 0;
                current_process_b.remainingTime = process.mmsg.runningtime;
                current_process_b.waitingTime = 0;

                pData[index++] = current_process_b;
                void *newpData = (struct pcb *)realloc(pData, (index + 1) * sizeof(struct pcb));

                /*
                    int excutionTime;
                */
            }
        }

        // for(int i = 0 ;i<index ; i++){
        //     printf("%d    \n", pData[i].process.id);
        // }

        // while (x == pData[head].arrivaltime)
        // {
        // printf("SENDING PROCESS %d \n", pData[head].id);
        // head = pData[head].id_next_process;
        // process.mtype = 0;
        // process.mmsg = pData[head];
        //     head ++;
        // }

        // while (x == getClk())
        // {
        // }
    }

    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    destroyClk(true);
}
