#include <string.h>
#include <stdio.h>
#include "headers.h"
#include "dataStructures.h"

struct msgbuff
{
    long mtype;
    processData mmsg;
};

void clearResources(int signum);

union Semun semun;
int sem1, sem2, sem3, shm_id_one;

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);

    char *algo = argv[1];
    printf("ARGC: %d\tARGUMENT SENT: %s\n", argc, algo);

    key_t msg_queue_key_id, shr_mem_id, sem2_id, sem3_id;
    int process_msgq_id;

    msg_queue_key_id = ftok("keyfile", 'a'); //create unique key
    shr_mem_id = ftok("keyfile", 'b');       //create unique key
    sem2_id = ftok("keyfile", 'c');          //create unique key
    sem3_id = ftok("keyfile", 'd');

    printf("ALGO CHOSEN: %s   %s  %s\n", argv[0], argv[1], argv[2]);

    process_msgq_id = msgget(msg_queue_key_id, 0666 | IPC_CREAT); //create message queue and return id
    shm_id_one = shmget(shr_mem_id, 256, IPC_CREAT | 0666);
    sem1 = semget(shr_mem_id, 1, 0666 | IPC_CREAT);
    sem2 = semget(sem2_id, 1, 0666 | IPC_CREAT);
    sem3 = semget(sem3_id, 1, 0666 | IPC_CREAT);


//open files
//FILE *fp_log = fopen("./scheduler.log", "w");
//fprintf(fp_log,"#At time x process y state arr w total z remain y wait k\n");






    if (process_msgq_id == -1 || shm_id_one == -1 || sem1 == -1 || sem2 == -1 || sem3 == -1)
    {
        perror("Error in create");
        exit(-1);
    }

    void *shmaddr = shmat(shm_id_one, (void *)0, 0);
    if (*(int *)shmaddr == -1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }
    semun.val = 0; /* initial value of the semaphore, Binary semaphore */
    if (semctl(sem1, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
    if (semctl(sem3, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
    semun.val = 1;

    if (semctl(sem2, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }

    initClk();
    queue readyQueue;
    initQueue(&readyQueue);
    struct msgbuff process;
    printf("Hello frm scheduler!\n");
    int index = 0;
    node *current_running_process = (node *)malloc(sizeof(node));
    current_running_process = NULL;
    //  pcb *pData = (struct pcb *)malloc(1 * sizeof( pcb));
    int currentTime = -1;
    while (1)
    {
        // To get time use this function.
        int x = getClk();

        int rec_val = 0;
        // To check if more than 1 process arrived at the same time step
        while (rec_val != -1)
        {

            rec_val = msgrcv(process_msgq_id, &process, sizeof(process.mmsg), 0, IPC_NOWAIT);

            // Check if any new process arrived to the scheduler
            if (rec_val != -1)
            {
                currentTime = -1;
                printf("Current Time is %d\n", x);
                printf("Message received in server: %d\n", process.mmsg.id);

                // Instanciate a new process control block for each process arriving from the process generator
                pcb *current_process_b = (pcb *)malloc(sizeof(pcb));
                current_process_b->process = process.mmsg;
                current_process_b->state = 0;
                current_process_b->remainingTime = process.mmsg.runningtime;
                current_process_b->waitingTime = 0;

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

                // Insert the new process into a queue according to the algorithm recieved
                insertQueue(&readyQueue, current_process_b, algo);

                /*
                    int excutionTime;
                */
            }
        }

        if (currentTime != x)
        {
            currentTime = x;
            if (atoi(algo) == 1)
            { /// FCFS
                if (!isEmpty(readyQueue) || current_running_process != NULL)
                {
                    if (current_running_process == NULL)
                    {
                        down(sem2);
                        node *nn = dequeue(&readyQueue);
                        current_running_process = nn;
                        up(sem1);
                    }
                    char *number_temp = malloc(sizeof(char));
                    int remaining_time = current_running_process->data->remainingTime--;
                    printf("FROM THE PARENT: %d\n", remaining_time);
                    sprintf(number_temp, "%d", remaining_time);
                    strcpy((char *)shmaddr, number_temp);
                    if (current_running_process->data->remainingTime <= 0)
                    {
                        current_running_process = NULL;
                        up(sem2);
                    }
                    up(sem3);
                }
            }
            else if (atoi(algo) == 2)
            { //SJF
            }
            else if (atoi(algo) == 3)
            { //HPF
            }
            else if (atoi(algo) == 4)
            { //SRTN
            }
        }
    }

    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.

    destroyClk(true);
    semctl(sem2, 0, IPC_RMID, semun);
    semctl(sem3, 0, IPC_RMID, semun);
    semctl(sem1, 0, IPC_RMID, semun);
}

void HighestPriorityFirst(queue* readyQueue,node* currentNode,int currentClk,FILE* fp_log)
{

    // I need to create a priority queue first







}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    printf("Clearing Resources in Scheduler...\n");
    semctl(sem2, 0, IPC_RMID, semun);
    semctl(sem3, 0, IPC_RMID, semun);
    semctl(sem1, 0, IPC_RMID, semun);
    shmctl(shm_id_one, IPC_RMID, (struct shmid_ds *)0);

    exit(0);
}