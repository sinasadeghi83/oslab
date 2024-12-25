#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_COUNT 10

int buffer = 0;
int reader_count = 0;
sem_t resource_access;
sem_t wait_reader;
sem_t wait_writer;
int stop = 0;

void *reader(void *arg)
{
    long id = (long)arg;
    int lastread = -1;
    while (1)
    {
        sem_wait(&wait_writer);
        if (stop)
        {
            sem_post(&wait_writer);
            break;
        }
        sem_wait(&resource_access);
        if (lastread == buffer)
        {
            sem_post(&wait_writer);
            sem_post(&resource_access);
            continue;
        }
        printf("Reader %ld reads buffer value: %d\n", id, buffer);
        lastread = buffer;
        // sleep(1);
        sem_post(&wait_reader);
        sem_post(&resource_access);
    }

    printf("Reader %ld stopped.\n", id);
    return NULL;
}

void *writer(void *arg)
{
    long id = (long)arg;
    while (1)
    {
        sem_wait(&resource_access);
        buffer++;
        printf("Writer %ld writes buffer value: %d\n", id, buffer);
        // sleep(1);
        sem_post(&resource_access);
        sem_post(&wait_writer);
        sem_wait(&wait_reader);
        sem_post(&wait_writer);
        sem_wait(&wait_reader);
        if (buffer == MAX_COUNT)
        {
            sem_post(&wait_writer);
            stop = 1;
            break;
        }
    }

    printf("Writer %ld stopped.\n", id);
    return NULL;
}

int main()
{
    pthread_t readers[2], writer_thread;

    sem_init(&resource_access, 0, 1);
    sem_init(&wait_reader, 0, 2);
    sem_init(&wait_writer, 0, 1);

    sem_wait(&wait_reader);
    sem_wait(&wait_reader);
    sem_wait(&wait_writer);

    pthread_create(&writer_thread, NULL, writer, (void *)1);
    pthread_create(&readers[0], NULL, reader, (void *)1);
    pthread_create(&readers[1], NULL, reader, (void *)2);

    pthread_join(readers[0], NULL);
    pthread_join(readers[1], NULL);

    // Clean up semaphores
    sem_destroy(&resource_access);
    sem_destroy(&wait_writer);
    sem_destroy(&wait_reader);

    return 0;
}
