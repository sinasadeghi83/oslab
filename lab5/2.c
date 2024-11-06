#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define NUM_PROCESSES 4 // Number of concurrent processes
#define SAMPLES_PER_PROCESS 100

void printHistogram(int *hist)
{
    int i, j;
    for (i = 0; i < 25; i++)
    {
        for (j = 0; j < hist[i]; j++)
        {
            printf("*");
        }
        printf("\n");
    }
}

void sample(int *hist, int semid)
{
    int counter = 0;
    for (int i = 0; i < 12; i++)
    {
        int random_num = rand() % 101; // Generate a random number between 0 and 100

        if (random_num >= 49)
        {
            counter++; // Increment counter if number is >= 49
        }
        else
        {
            counter--; // Decrement counter otherwise
        }
    }

    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);

    // Update the shared histogram based on counter value
    if (counter >= -12 && counter <= 12)
    {
        hist[counter + 12]++; // Adjust index to fit within 0 to 24
    }

    sem_op.sem_op = 1;
    semop(semid, &sem_op, 1);
}

int main()
{
    // Create a unique key for the shared memory
    key_t key = ftok("shmfile", 65);                             // Generate a unique key
    int shmid = shmget(key, sizeof(int) * 25, 0666 | IPC_CREAT); // Create shared memory segment

    if (shmid < 0)
    {
        perror("shmget");
        exit(1);
    }

    // Attach the shared memory segment
    int *hist = shmat(shmid, NULL, 0);
    if (hist == (int *)-1)
    {
        perror("shmat");
        exit(1);
    }

    // Initialize the shared histogram
    memset(hist, 0, sizeof(int) * 25);

    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (semid < 0)
    {
        perror("semget");
        exit(1);
    }

    semctl(semid, 0, SETVAL, 1);

    // Start time measurement
    clock_t start_time = clock();

    pid_t pids[NUM_PROCESSES];

    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        pids[i] = fork(); // Create a new process

        if (pids[i] < 0)
        {
            perror("Fork failed");
            exit(1);
        }
        else if (pids[i] == 0)
        {
            srand(i); // Seed random number generator with process ID
            // Child process
            for (int smp = 0; smp < SAMPLES_PER_PROCESS; smp++)
            {
                sample(hist, semid); // Perform sampling
            }
            exit(0); // Child process exits
        }
    }

    // Parent process waits for all children to finish
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        wait(NULL);
    }

    // End time measurement
    clock_t end_time = clock();

    // Calculate elapsed time in seconds
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Print results
    printf("Hist array:\n");
    for (int i = 0; i < 25; i++)
    {
        printf("hist[%d]: %d\n", i - 12, hist[i]); // Display hist values with adjusted index
    }

    printf("\n\n");
    printHistogram(hist);
    printf("\n\n");

    // Print the runtime
    printf("Elapsed time: %.6f seconds\n", elapsed_time);

    // Detach and remove the shared memory
    shmdt(hist);
    shmctl(shmid, IPC_RMID, NULL); // Mark the segment for deletion
    semctl(semid, 0, IPC_RMID);
    return 0;
}