#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 16384  /* make it a 2*64bit shared memory segment */

int main(int argc, char *argv[])
{
    key_t key;
    int shmid;    

    key = ftok("test.txt", 2);
    if(key == -1){
        perror("ftok");
        exit(1);
    }

    shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT);
    if(shmid == -1){
        perror("shmget");
        exit(1);
    }

    int *data = (int *) shmat(shmid, 0, 0);
    if(data == (void *)-1){
        perror("shmat");
        exit(1);
    }

    int sum = 0;
    while(1){
        while(data[0] == 0){
            //waiting
        }
        if(data[0] == -1){
            break;
        }
        printf("Consumer: %d\tSum=%d\n", data[1], sum+=data[1]);
        data[0] = 0;
    }


    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}