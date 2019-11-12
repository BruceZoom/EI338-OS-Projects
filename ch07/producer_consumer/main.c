#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "buffer.h"

#define MAX_SLEEP_PERIOD 3

void *producer(void *param){
    buffer_item item;

    while(1){
        // sleep for a random period of time
        sleep(rand() % MAX_SLEEP_PERIOD + 1);
        // generate a random number
        item = rand();
        if(insert_item(item))
            fprintf(stderr, "error in producer");
        else
            printf("producer produced %d\n", item);
    }
}

void *consumer(void *param){
    buffer_item item;

    while(1){
        // sleep for a random period of time
        sleep(rand() % MAX_SLEEP_PERIOD + 1);
        // generate a random number
        item = rand();
        if(remove_item(&item))
            fprintf(stderr, "error in consumer");
        else
            printf("consumer consumed %d\n", item);
    }
}

int main(int argc, char *argv[]){
    int sleep_time = 15;
    int n_producer = 3;
    int n_consumer = 3;
    pthread_t *producers;
    pthread_t *consumers;
    int i;

    // get command line input
    if(1 < argc) sleep_time = atoi(argv[1]);
    if(2 < argc) n_producer = atoi(argv[2]);
    if(3 < argc) n_consumer = atoi(argv[3]);

    // initialize buffer and memory for thread identifiers
    init();
    producers = (pthread_t*)malloc(sizeof(pthread_t) * n_producer);
    consumers = (pthread_t*)malloc(sizeof(pthread_t) * n_consumer);

    // create producers and consumers
    for(i = 0; i < n_producer; i++){
        pthread_create(&producers[i], NULL, producer, NULL);
    }
    for(i = 0; i < n_consumer; i++){
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    // sleep
    sleep(sleep_time);

    // cancel and join threads
    for(i = 0; i < n_producer; i++){
        pthread_cancel(producers[i]);
        pthread_join(producers[i], NULL);
    }
    for(i = 0; i < n_consumer; i++){
        pthread_cancel(consumers[i]);
        pthread_join(consumers[i], NULL);
    }

    // clean up and exit
    free(producers);
    free(consumers);
    return 0;
}
