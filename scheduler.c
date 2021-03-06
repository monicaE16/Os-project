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
void sheduler_logger(int, node *);
// void handler (int signum);

union Semun semun;
int sem1, sem2, sem3, sem4, shm_id_one;
node *current_running_process;
FILE *fp_log;
FILE *fp_PERF;
int No_proc = 0;    //real no. of processes in text file
int counter_proc=0; // count of finish processes

int last_clk=0;
int totalWait=0;
float totalWTA=0.0;
int totalExectution=0;


int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // signal(SIGUSR1, handler);

    // Open file
    fp_log = fopen("./scheduler.log", "w");
    fprintf(fp_log, "#At\ttime\tx\tprocess\ty\tstate    \t\tarr\tw\ttotal\tz\tremain\ty\twait\tk\n");

    char *algo = argv[1];
    char *quantum = argv[2];
    printf("ARGC: %d\tARGUMENT SENT: %s   %s\n", argc, algo, quantum);

    key_t msg_queue_key_id, shr_mem_id, sem2_id, sem3_id, sem4_id;
    int process_msgq_id;

    msg_queue_key_id = ftok("keyfile", 'a'); //create unique key
    shr_mem_id = ftok("keyfile", 'b');       //create unique key
    sem2_id = ftok("keyfile", 'c');          //create unique key
    sem3_id = ftok("keyfile", 'd');
    sem4_id = ftok("keyfile", 'e');

    printf("ALGO CHOSEN: %s   %s  %s\n", argv[0], argv[1], argv[2]);

    process_msgq_id = msgget(msg_queue_key_id, 0666 | IPC_CREAT); //create message queue and return id
    shm_id_one = shmget(shr_mem_id, 256, IPC_CREAT | 0666);
    sem1 = semget(shr_mem_id, 1, 0666 | IPC_CREAT);
    sem2 = semget(sem2_id, 1, 0666 | IPC_CREAT);
    sem3 = semget(sem3_id, 1, 0666 | IPC_CREAT);
    sem4 = semget(sem4_id, 1, 0666 | IPC_CREAT);

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
    // if (semctl(sem3, 0, SETVAL, semun) == -1)
    // {
    //     perror("Error in semctl");
    //     exit(-1);
    // }
    // semun.val = 1;

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
    current_running_process = (node *)malloc(sizeof(node));
    current_running_process = NULL;
    char *number_temp = malloc(sizeof(char));
    //  pcb *pData = (struct pcb *)malloc(1 * sizeof( pcb));
    int currentTime = -1;
    int q;
    if (quantum != NULL)
    {

        q = atoi(quantum);
    }
    else
    {
        perror("Not enough arguments !");
    }
    int local_quantum = q;
    bool quantumFinished = false;
    while (1)
    {
        // To get time use this function.
        down(sem3);
        int x = getClk();

        int rec_val = 0;
        // To check if more than 1 process arrived at the same time step
        // down(sem3);
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
                if (No_proc == 0) //getting number of processes for one time only
                {
                    No_proc = current_process_b->process.global_N;
                    printf("******** NO of processes %d in line 140 ******** \n",No_proc);
                }
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

                current_process_b->pid = pid_process;
                // Insert the new process into a queue according to the algorithm recieved
                insertQueue(&readyQueue, current_process_b, algo, quantum);
                kill(pid_process, SIGSTOP);

                /*
                    int excutionTime;
                */
            }
        }

        if (currentTime != x)
        {
            currentTime = x;
            if (atoi(algo) == 1) //--------------------------------------------->FCFS
            {                    /// FCFS
                if (!isEmpty(readyQueue) || current_running_process != NULL)
                {
                    if (current_running_process == NULL)
                    {
                        node *nn = dequeue(&readyQueue);
                        current_running_process = nn;
                        current_running_process->data->startTime = currentTime; //have to check state == 0
                        sheduler_logger(currentTime, current_running_process);
                        char *number_temp = malloc(sizeof(char));
                        int remaining_time = current_running_process->data->remainingTime;
                        printf("FROM THE PARENT: %d\n", remaining_time);
                        sprintf(number_temp, "%d", remaining_time);
                        strcpy((char *)shmaddr, number_temp);
                        kill(current_running_process->data->pid, SIGCONT);
                    }

                    char *number_temp = malloc(sizeof(char));
                    down(sem2);
                    strcpy(number_temp, (char *)shmaddr);
                    current_running_process->data->remainingTime = atoi(number_temp);
                    if (current_running_process->data->remainingTime <= 0)
                    {
                        current_running_process->data->state = 3;
                        sheduler_logger(currentTime + 1, current_running_process);
                        current_running_process = NULL;
                    }
                }
            }
            else if (atoi(algo) == 2) //-------------------------------------------->SJF
            {
                if (!isEmpty(readyQueue) || current_running_process != NULL)
                {
                    if (current_running_process == NULL)
                    {
                        node *nn = dequeue(&readyQueue);

                        current_running_process = nn;
                        current_running_process->data->startTime = currentTime; //have to check state == 0
                        sheduler_logger(currentTime, current_running_process);
                        char *number_temp = malloc(sizeof(char));
                        int remaining_time = current_running_process->data->remainingTime;

                        printf("FROM THE PARENT: %d\n", remaining_time);
                        sprintf(number_temp, "%d", remaining_time);
                        strcpy((char *)shmaddr, number_temp);
                        kill(current_running_process->data->pid, SIGCONT);
                    }

                    char *number_temp = malloc(sizeof(char));
                    down(sem2);
                    strcpy(number_temp, (char *)shmaddr);
                    current_running_process->data->remainingTime = atoi(number_temp);
                    if (current_running_process->data->remainingTime <= 0)
                    {

                        current_running_process->data->state = 3;
                        sheduler_logger(currentTime + 1, current_running_process);
                        current_running_process = NULL;
                    }
                }
            }
            else if (atoi(algo) == 3)
            { //HPF

                if (!isEmpty(readyQueue) || current_running_process != NULL)
                {
                    printf("\n\nCLK TIME: %d\n", x);
                    node *peeker = peek(&readyQueue);

                    if (current_running_process == NULL)
                    {
                        printList(&readyQueue);
                        node *n = dequeue(&readyQueue);
                        current_running_process = n;
                        if (current_running_process->data->state == 0)
                        {
                            current_running_process->data->startTime = currentTime; //have to check state == 0
                            printf("SCH: PROCESS %d STARTED\n", current_running_process->data->process.id);
                            char *firstRemStr = (char *)malloc(sizeof(char));
                            sprintf(firstRemStr, "%d", current_running_process->data->remainingTime);
                            strcpy((char *)shmaddr, firstRemStr);
                        }
                        else if (current_running_process->data->state == 2)
                        {
                            current_running_process->data->state = 1;
                            printf("SCH: PROCESS %d RESUMED %d\n", current_running_process->data->process.id, current_running_process->data->pid);
                        }
                        kill(current_running_process->data->pid, SIGCONT);
                        sheduler_logger(x, current_running_process);
                    }
                    else if (peeker != NULL)
                    {

                        if (peeker->data->process.priority < current_running_process->data->process.priority)
                        {
                            printf("HELLO 1\n");
                            kill(current_running_process->data->pid, SIGSTOP);
                            current_running_process->data->state = 2;
                            sheduler_logger(x, current_running_process);
                            printf("HELLO 2\n");

                            printf("SCH: PROCESS %d STOPPED\n", current_running_process->data->process.id);
                            printList(&readyQueue);
                            peeker = dequeue(&readyQueue);
                            enqueue_priority(&readyQueue, current_running_process);
                            printList(&readyQueue);
                            current_running_process = peeker;
                            peeker = NULL;
                            if (current_running_process->data->state == 2)
                            {
                                printf("SCH: PROCESS %d RESUMED FROM PEEEEEEEEEEEEEEEEEK\n", current_running_process->data->process.id);
                                current_running_process->data->state = 1;
                            }
                            else if (current_running_process->data->state == 0)
                            {
                                //STATE 0 ==> STARTED
                                current_running_process->data->startTime = currentTime; //have to check state == 0
                                printf("SCH: PROCESS %d STARTED FROM PEEEEEEEEEEEEEEEEEK\n", current_running_process->data->process.id);
                                char *firstRemStr = (char *)malloc(sizeof(char));
                                sprintf(firstRemStr, "%d", current_running_process->data->remainingTime);
                                strcpy((char *)shmaddr, firstRemStr);
                            }
                            kill(current_running_process->data->pid, SIGCONT);
                            sheduler_logger(x, current_running_process);
                        }
                    }
                    char *remainingStr = malloc(sizeof(char));
                    down(sem2);
                    strcpy(remainingStr, (char *)shmaddr);
                    current_running_process->data->remainingTime = atoi(remainingStr);
                    if (current_running_process->data->remainingTime <= 0)
                    {

                        current_running_process->data->state = 3;
                        sheduler_logger(x + 1, current_running_process);

                        current_running_process = NULL;
                    }
                    // if (current_running_process->data->remainingTime <= 0)
                    // {
                    // }
                    // else
                    // {

                    // }
                }
            }
            else if (atoi(algo) == 4)
            { //SRTN

                if (!isEmpty(readyQueue) || current_running_process != NULL)
                {
                    printf("\n\nCLK TIME: %d\n", x);
                    node *peeker = peek(&readyQueue);

                    if (current_running_process == NULL)
                    {
                        printList(&readyQueue);
                        node *n = dequeue(&readyQueue);
                        current_running_process = n;

                        if (current_running_process->data->state == 0)
                        {
                            current_running_process->data->startTime = currentTime; //have to check state == 0
                            printf("SCH: PROCESS %d STARTED\n", current_running_process->data->process.id);
                            char *firstRemStr = (char *)malloc(sizeof(char));
                            sprintf(firstRemStr, "%d", current_running_process->data->remainingTime);
                            strcpy((char *)shmaddr, firstRemStr);
                            // up(sem1);
                        }
                        else if (current_running_process->data->state == 2)
                        {
                            current_running_process->data->state = 1;
                            printf("SCH: PROCESS %d RESUMED %d\n", current_running_process->data->process.id, current_running_process->data->pid);
                        }
                        kill(current_running_process->data->pid, SIGCONT);
                        sheduler_logger(x, current_running_process);
                    }
                    else if (peeker != NULL)
                    {

                        if (peeker->data->remainingTime < current_running_process->data->remainingTime)
                        {
                            printf("HELLO 1\n");
                            kill(current_running_process->data->pid, SIGSTOP);
                            current_running_process->data->state = 2;
                            printf("HELLO 2\n");
                            sheduler_logger(x, current_running_process);

                            printf("SCH: PROCESS %d STOPPED\n", current_running_process->data->process.id);
                            printList(&readyQueue);
                            peeker = dequeue(&readyQueue);
                            enqueue_rem_time(&readyQueue, current_running_process);
                            printList(&readyQueue);
                            current_running_process = peeker;
                            peeker = NULL;
                            if (current_running_process->data->state == 2)
                            {
                                printf("SCH: PROCESS %d RESUMED FROM PEEEEEEEEEEEEEEEEEK\n", current_running_process->data->process.id);
                                current_running_process->data->state = 1;
                            }
                            else if (current_running_process->data->state == 0)
                            {
                                current_running_process->data->startTime = currentTime; //have to check state == 0
                                //STATE 0 ==> STARTED
                                printf("SCH: PROCESS %d STARTED FROM PEEEEEEEEEEEEEEEEEK\n", current_running_process->data->process.id);
                                char *firstRemStr = (char *)malloc(sizeof(char));
                                sprintf(firstRemStr, "%d", current_running_process->data->remainingTime);
                                strcpy((char *)shmaddr, firstRemStr);
                            }
                            kill(current_running_process->data->pid, SIGCONT);

                            sheduler_logger(x, current_running_process);
                        }
                    }
                    char *remainingStr = malloc(sizeof(char));
                    down(sem2);
                    strcpy(remainingStr, (char *)shmaddr);
                    printf("*****from scheduler I received my remainigTime %d *****\n", atoi(remainingStr));
                    current_running_process->data->remainingTime = atoi(remainingStr);
                    if (current_running_process->data->remainingTime <= 0)
                    {

                        current_running_process->data->state = 3;
                        sheduler_logger(x + 1, current_running_process);

                        current_running_process = NULL;
                    }
                }
            }
            else if (atoi(algo) == 5)
            {
                if (!isEmpty(readyQueue) || current_running_process != NULL)
                {
                    if (quantumFinished)
                    {
                        quantumFinished = false;
                        current_running_process->data->state = 2;
                        sheduler_logger(x, current_running_process);
                        enqueue(&readyQueue, current_running_process);
                        current_running_process = NULL;
                    }
                    if (current_running_process == NULL)
                    {
                        local_quantum = q;
                        node *nn = dequeue(&readyQueue);
                        current_running_process = nn;
                        if (current_running_process->data->state == 2)
                        {
                            current_running_process->data->state = 1;
                        }
                        else
                        {

                            char *number_temp = malloc(sizeof(char));
                            int remaining_time = current_running_process->data->remainingTime;
                            printf("FROM THE PARENT: %d\n", remaining_time);
                            sprintf(number_temp, "%d", remaining_time);
                            strcpy((char *)shmaddr, number_temp);
                        }

                        sheduler_logger(x, current_running_process);
                        kill(current_running_process->data->pid, SIGCONT);
                        // up(sem1);
                    }

                    char *number_temp = malloc(sizeof(char));
                    current_running_process->data->waitingTime = x - current_running_process->data->startTime - (current_running_process->data->process.runningtime - current_running_process->data->remainingTime);
                    down(sem2);
                    strcpy(number_temp, (char *)shmaddr);
                    current_running_process->data->remainingTime = atoi(number_temp);
                    local_quantum--;
                    if (local_quantum <= 0 || current_running_process->data->remainingTime <= 0)
                    {
                        if (!isEmpty(readyQueue))
                        {
                            printList(&readyQueue);
                            if (current_running_process->data->remainingTime <= 0)
                            {
                                current_running_process->data->state = 3;
                                sheduler_logger(x + 1, current_running_process);
                                current_running_process = NULL;
                            }
                            else
                            {
                                // printList(&readyQueue);
                                quantumFinished = true;
                                // enqueue(&readyQueue, current_running_process);

                                current_running_process->data->state = 2;
                                kill(current_running_process->data->pid, SIGSTOP);

                                // current_running_process = NULL;
                            }
                        }
                        else
                        {

                            local_quantum = q;
                            if (current_running_process->data->remainingTime <= 0)
                            {
                                current_running_process->data->state = 3;
                                sheduler_logger(x + 1, current_running_process);
                                current_running_process = NULL;
                            }
                            else
                            {
                                quantumFinished = true;
                                kill(current_running_process->data->pid, SIGSTOP);
                            }
                        }
                    }
                }
            }
        }
    }

    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
    fclose(fp_log);

    destroyClk(true);
    semctl(sem2, 0, IPC_RMID, semun);
    semctl(sem3, 0, IPC_RMID, semun);
    semctl(sem1, 0, IPC_RMID, semun);
}

void HighestPriorityFirst(queue *readyQueue, node *currentNode, int currentClk, FILE *fp_log)
{

    // I need to create a priority queue first
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    printf("Clearing Resources in Scheduler...\n");
    fclose(fp_log);
    semctl(sem2, 0, IPC_RMID, semun);
    semctl(sem3, 0, IPC_RMID, semun);
    semctl(sem1, 0, IPC_RMID, semun);
    shmctl(shm_id_one, IPC_RMID, (struct shmid_ds *)0);

    exit(0);
}

void sheduler_logger(int currentTime, node *n)
{
    pcb *nPCB = n->data;
    processData *nPD = &n->data->process;
    char *nState = (char *)malloc(20);

    nPCB->waitingTime = currentTime - nPD->arrivaltime - (nPD->runningtime - nPCB->remainingTime);
    //nPCB->state
    switch (nPCB->state)
    {
    case 0:
        strcpy(nState, "started ");
        break;
    case 1:
        strcpy(nState, "resumed ");
        break;
    case 2:
        strcpy(nState, "stopped ");
        break;
    case 3:
        strcpy(nState, "finished");
        break;
    }

    if (nPCB->state != 3)
        fprintf(fp_log, "At\ttime\t%d\tprocess\t%d\t%s\t\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", currentTime, nPD->id + 1, nState, nPD->arrivaltime, nPD->runningtime, nPCB->remainingTime, nPCB->waitingTime);
    else
    {
        int TA = currentTime - nPD->arrivaltime;
        float WTA = (float)TA / ((float)nPD->runningtime);
        totalWait+=nPCB->waitingTime;
        totalWTA+=WTA;
        last_clk=currentTime-1;
        totalExectution+=nPD->runningtime;
        counter_proc++;
        // printf("TA: %d\tRunTime: %.2f\n", TA, nPD->runningtime);
        fprintf(fp_log, "At\ttime\t%d\tprocess\t%d\t%s\t\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA %d\tWTA %9.2f\n", currentTime, nPD->id + 1, nState, nPD->arrivaltime, nPD->runningtime, nPCB->remainingTime, nPCB->waitingTime, TA, WTA);
    }
    free(nState);

if(counter_proc==No_proc) 
{
    printf("\n we have finished the processes \n");
    //writing in scheduler.perf

       float avg_waiting = (float)totalWait /((float)No_proc);
       float avg_WTA = (float)totalWTA /((float)No_proc);
       float cpu_util=(float)totalExectution /((float)last_clk);

        fp_PERF = fopen("./scheduler.perf", "w");
        fprintf(fp_PERF, "CPU utilization %.2f %% \n",cpu_util*100.00);

        fprintf(fp_PERF, "Avg WTA = %.2f  \n", avg_WTA);
        fprintf(fp_PERF, "Avg Waiting = %.2f  \n", avg_waiting);

        fclose(fp_PERF);







   kill(getppid(), SIGINT);
}



}

