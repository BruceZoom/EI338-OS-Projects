#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include "buffer.h"

// the buffer
buffer_item buffer[BUFFER_SIZE];
static int tail = 0;

pthread_mutex_t mutex;
sem_t empty;
sem_t full;

// insert item into buffer
// return 0 if successful, otherwise
// return -1 indicating an error condition
int insert_item(buffer_item item){
    int flag;
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    // insert item
    if(tail < BUFFER_SIZE){
        buffer[tail++] = item;
        flag = 0;
    }
    else{
        flag = -1;
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    return flag;
}

// remove an object from buffer
// placing it in item
// return 0 if successful, otherwise
// return -1 indicating an error condition
int remove_item(buffer_item *item){
    int flag;
    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    // remove item
    if(tail > 0){
        *item = buffer[--tail];
        flag = 0;
    }
    else{
        flag = -1;
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    return flag;
}

void init(){
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
}
