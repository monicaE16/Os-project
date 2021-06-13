#include "headers.h"
#include "dataStructures.h"
#include <string.h>

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();
    printf("HELLO FROM CHILD %d\n", getpid());
    key_t shr_mem_id = ftok("keyfile", 'b');
    key_t sem3_id = ftok("keyfile", 'd'); 
    int shm_id_one = shmget(shr_mem_id, 256, IPC_CREAT | 0666);
    int sem1 = semget(shr_mem_id, 1, 0666 | IPC_CREAT);
    int sem3 = semget(sem3_id, 1, 0666 | IPC_CREAT);

    union Semun semun;

    void *shmaddr = shmat(shm_id_one, (void *)0, 0);
    if (*(int *)shmaddr == -1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }

    


    // semun.val = 0; /* initial value of the semaphore, Binary semaphore */
    // if (semctl(sem1, 0, SETVAL, semun) == -1)
    // {
    //     perror("Error in semctl");
    //     exit(-1);
    // }
    // printf("------------------------------------BEFORE INIT----------------\n");

    //TODO The process needs to get the remaining time from somewhere
    //remainingtime = ??;
    down(sem1);
    char *rmtime = malloc(sizeof(char));
    strcpy(rmtime, (char *)shmaddr);
    remainingtime = atoi(rmtime);
    // printf("------------------------------------BEFORE THE WHILE LOOP----------------\n");
    while (remainingtime > 0)
    {
        down(sem3);

        strcpy(rmtime, (char *)shmaddr);
        remainingtime = atoi(rmtime);
        printf("\n I'M A CHILD! MY REMAINING TIME = %d %d\n", remainingtime,getpid());
        // sleep(1);

        // remainingtime = ??;
    }
    printf("PROC %d EXITING...\n",getpid());
    destroyClk(false);

    return 0;
}
