#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

sem_t chopsticks[NUM_PHILOSOPHERS];

void *philosopher(void *num) {
    int id = *(int *)num;

    while (1) {
        printf("Philosopher %d is thinking...\n", id);
        sleep(1);


        printf("Philosopher %d is hungry and trying to pick up chopsticks.\n", id);
        printf("Philosopher %d is trying to pick up left chopstick.\n", id);
        sem_wait(&chopsticks[id]); 
        printf("Philosopher %d picked up left chopstick.\n", id);
        sem_wait(&chopsticks[(id + 1) % NUM_PHILOSOPHERS]); 
        printf("Philosopher %d picked up right chopstick.\n", id);

     
        printf("Philosopher %d is eating.\n", id);
        sleep(2);


        printf("Philosopher %d put down left chopstick.\n", id);
        sem_post(&chopsticks[id]);
        printf("Philosopher %d put down right chopstick.\n", id);
        sem_post(&chopsticks[(id + 1) % NUM_PHILOSOPHERS]); 

        printf("Philosopher %d has finished eating and is thinking again.\n", id);
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

 
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&chopsticks[i], 0, 1);
        ids[i] = i;
    }


    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }


    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(threads[i], NULL);
    }


    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&chopsticks[i]);
    }

    return 0;
}