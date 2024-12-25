#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_COUNT 10

int buffer = 0;            
int reader_count = 0;       
sem_t resource_access;      
sem_t reader_count_access; 
int stop = 0;
int stop_reader[2];
int isread[2];               

void *reader(void *arg) {
    long id = (long)arg;
    while (1) {

        if (stop_reader[id]) {
            sem_post(&reader_count_access);
            break;
        }
        
        if (reader_count == 1) {
            sem_wait(&resource_access); 
        }

        printf("Reader %ld reads buffer value: %d\n", id, buffer);
        isread[id] = 1;
        if(stop){
            stop_reader[id] = 1;
        }
        sleep(1);


        if (isread[0] && isread[1]) {
            isread[0] = isread[1] = 0;
            sem_post(&resource_access);
        }

    }

    printf("Reader %ld stopped.\n", id);
    return NULL;
}

void *writer(void *arg) {
    long id = (long)arg;
    while (1) {
        sem_wait(&resource_access);
        if (buffer >= MAX_COUNT) {
            stop = 1;
            sem_post(&resource_access);
            break;
        }

        buffer++;
        printf("Writer %ld writes buffer value: %d\n", id, buffer);
        sleep(1);

        sem_post(&resource_access);
    }

    printf("Writer %ld stopped.\n", id);
    return NULL;
}

int main() {
    pthread_t readers[2], writer_thread;

    sem_init(&resource_access, 0, 1);
    sem_init(&reader_count_access, 0, 1);

    pthread_create(&writer_thread, NULL, writer, (void *)1);
    pthread_create(&readers[0], NULL, reader, (void *)1);
    pthread_create(&readers[1], NULL, reader, (void *)2);

    pthread_join(readers[0], NULL);
    pthread_join(readers[1], NULL);

    // Clean up semaphores
    sem_destroy(&resource_access);
    sem_destroy(&reader_count_access);

    return 0;
}
