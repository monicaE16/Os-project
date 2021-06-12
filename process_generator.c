#include <unistd.h>
#include <string.h>
#include "headers.h"

void clearResources(int);

struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    // int id_next_process;
    // int id_prev_process;
};

struct msgbuff
{
    long mtype;
    // char mtext[256];
    struct processData mmsg;
};

bool RearrangeByArrivalTime(struct processData *, int, int *, int);
struct processData *pData;
int process_msgq_id;

int main(int argc, char *argv[])
{
    printf("\n");
    signal(SIGINT, clearResources);

    // TODO Initialization
    // 1. Read the input files.
    FILE *fp = fopen("./processes.txt", "r");

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    key_t process_key_id;

    process_key_id = ftok("keyfile", 'a'); //create unique key

    process_msgq_id = msgget(process_key_id, 0666 | IPC_CREAT); //create message queue and return id

    if (process_msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }

    int pid_clk, pid_scheduler;
    if (fp == NULL)
    {
        printf("Unable to open the file ... ");
        return -1;
    }

    // Count the number of lines in the file
    int count = 0;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (line[0] != '#')
        {
            count++;
        }
    }

    fclose(fp);
    fp = fopen("./processes.txt", "r");

    // Creating a dynamic array of size of lines read from the file
    pData = (struct processData *)malloc(len * sizeof(struct processData));
    int index = 0;
    // int head = 0;
    // int tail = -1;

    while ((read = getline(&line, &len, fp)) != -1)
    {
        printf("Retrieved line of length %zu:\n", read);
        if (line[0] != '#')
        {
            // Translate each line in the file into process parameter
            // Then add this process to the processes array
            struct processData current_process;
            int id, arrival_time, runtime, priority;

            printf("%s", line);
            sscanf(line, "%d\t%d\t%d\t%d", &id, &arrival_time, &runtime, &priority);
            current_process.id = id - 1;
            current_process.arrivaltime = arrival_time;
            current_process.runningtime = runtime;
            current_process.priority = priority;
            // current_process.id_next_process = -1;
            // current_process.id_prev_process = tail;
            pData[index++] = current_process;

            // Rearrange the array by the arrival time of the processes
            // ?bool changed = RearrangeByArrivalTime(pData, index, &head, tail);
            // if (!changed)
            // {
            //     tail= index-1;
            // }
        }
    }

    fclose(fp);
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    char *algoChosen = (char *)malloc(100);
    int quantum = -1;

    for (int i = 2; i < argc; i++)
    {
        if (!strcmp(argv[i], "-sch"))
        {
            strcpy(algoChosen, argv[i + 1]);
        }
        if (argv[i] == "-q")
        {
            quantum = atoi(argv[i + 1]);
        }
    }
    // if (algoChosen < 0 || algoChosen >5)
    // {
    //     printf("Please rerun the program & enter a scheduling algorithm");
    //     return -1;
    // }

    // 3. Initiate and create the scheduler and clock processes.

    // Fork a process for the clock
    pid_clk = fork();

    if (pid_clk == -1)
    {
        perror("error in fork");
    }
    else if (pid_clk == 0) // If the process is a child
    {
        // Execute the file clk.out
        char *const argv_clk[] = {"./clk.out", NULL};
        if (execvp("./clk.out", argv_clk) == -1)
        {
            printf("error in executing the file");
        }
    }

    // Fork a process for the scheduler
    pid_scheduler = fork();

    if (pid_scheduler == -1)
    {
        perror("error in fork");
    }
    else if (pid_scheduler == 0) // If the process is a child
    {

        // Execute the file scheduler.out
        printf("FROM PROCESS GENE. UP 22: %s \n", algoChosen);
        char *const argv_scheduler[] = {"./scheduler.out", algoChosen, NULL};
        if (execv(argv_scheduler[0], argv_scheduler) == -1)
        {
            printf("error in executing the file");
        }
    }

    // 4. Use this function after creating the clock process to initialize clock.
    initClk();
    int head = 0;

    struct msgbuff process;

    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    while (1)
    {
        // To get time use this function.
        int x = getClk();

        printf("Current Time is %d\n", x);
        while (x == pData[head].arrivaltime)
        {
            printf("SENDING PROCESS %d \n", pData[head].id);
            // head = pData[head].id_next_process;
            process.mtype = pid_scheduler;
            process.mmsg = pData[head];
            int send_val = msgsnd(process_msgq_id, &process, sizeof(process.mmsg), IPC_NOWAIT);
            head++;
        }

        while (x == getClk())
        {
        }
    }

    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    free(pData);
    msgctl(process_msgq_id, IPC_RMID, (struct msqid_ds *)0);

    exit(0);
}

// bool RearrangeByArrivalTime(struct processData *pData, int index, int *head, int tail)
// {
//     if (index == 1)
//         return false;
//     struct processData newNodeID = pData[index - 1];
//     if (newNodeID.arrivaltime >= pData[index - 2].arrivaltime)
//     {
//         newNodeID.id_prev_process = tail;
//         pData[tail].id_next_process = newNodeID.id;

//         return false;
//     }
//     int temp_index = tail;

//     // 2 6 5 1
//     while (pData[temp_index].arrivaltime > newNodeID.arrivaltime && temp_index >= 0)
//     {
//         temp_index = pData[temp_index].id_prev_process;
//         printf("TEMP INDEX: %d ", temp_index);
//     }
//     if (temp_index == -1)
//     {
//         pData[*head].id_prev_process = newNodeID.id;
//         newNodeID.id_next_process = pData[*head].id;
//         *head = newNodeID.id;
//         return true;
//     }

//     newNodeID.id_next_process = pData[temp_index ].id;
//     newNodeID.id_prev_process = pData[temp_index-1].id;
//     pData[temp_index].id_next_process = newNodeID.id;
//     pData[temp_index + 1].id_prev_process = newNodeID.id;
//     return true;
// }