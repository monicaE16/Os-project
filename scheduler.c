#include "headers.h"
#include "dataStructures.h"

struct msgbuff
{
    long mtype;
    // char mtext[256];
     processData mmsg;
};

int main(int argc, char *argv[])
{
printf("ARGC: %d", argc);

    for(int i= 0;i<argc;i++){
        printf("%s     ", argv[i]);
    }

    key_t process_key_id;
    int process_msgq_id;

    process_key_id = ftok("keyfile", 'a'); //create unique key
    
    printf("ALGO CHOSEN: %s   %s  %s\n",argv[0], argv[1], argv[2]);

    process_msgq_id = msgget(process_key_id, 0666 | IPC_CREAT); //create message queue and return id
    if (process_msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }

    initClk();
    struct msgbuff process;
    printf("Hello frm scheduler!");
    int index = 0;
     pcb *pData = (struct pcb *)malloc(1 * sizeof( pcb));
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
                printf("\nMessage received in server: ---------------------------------------------------------------------------------%d\n", process.mmsg.id);
                struct pcb current_process_b;
                current_process_b.process = process.mmsg;
                current_process_b.state = 0;
                current_process_b.remainingTime = process.mmsg.runningtime;
                current_process_b.waitingTime = 0;
                
                pData[index++] = current_process_b;
                void *newpData = ( pcb *)realloc(pData,(index+1) * sizeof( pcb));

                /*
                    int excutionTime;
                */
            }
        }

     






    }

    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    destroyClk(true);
}

void HighestPriorityFirst()
{

// I need to create a priority queue first 






}