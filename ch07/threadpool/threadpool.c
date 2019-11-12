/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
static task taskqueue[QUEUE_SIZE];
static int head = 0;
static int tail = 0;
static int cnt = 0;
pthread_mutex_t queue_mutex;
sem_t cnt_sem;

// the worker bee
pthread_t bees[NUMBER_OF_THREADS];

int isFull(){
    return cnt == QUEUE_SIZE;
}

int isEmpty(){
    return cnt == 0;
}

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    pthread_mutex_lock(&queue_mutex);
    if(isFull()){
        pthread_mutex_unlock(&queue_mutex);
        return 1;
    }
    taskqueue[tail++] = t;
    tail %= QUEUE_SIZE;
    cnt++;
    pthread_mutex_unlock(&queue_mutex);
    return 0;
}

// remove a task from the queue
task dequeue() 
{
    pthread_mutex_lock(&queue_mutex);
    task tmp = taskqueue[head++];
    head %= QUEUE_SIZE;
    cnt--;
    pthread_mutex_unlock(&queue_mutex);
    return tmp;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    task worktodo;
    while(TRUE){
        // execute the task
        sem_wait(&cnt_sem);
        worktodo = dequeue();
        execute(worktodo.function, worktodo.data);
    }

    // pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    int flag;
    task worktodo;
    worktodo.function = somefunction;
    worktodo.data = p;
    flag = enqueue(worktodo);
    if(flag) return flag;
    sem_post(&cnt_sem);
    return flag;
}

// initialize the thread pool
void pool_init(void)
{
    int i;
    sem_init(&cnt_sem, 0, 0);
    pthread_mutex_init(&queue_mutex, NULL);
    for(i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_create(&bees[i],NULL,worker,NULL);
}

// shutdown the thread pool
void pool_shutdown(void)
{
    int i;
    for(i = 0; i < NUMBER_OF_THREADS; i++)
    {
        pthread_cancel(bees[i]);
        pthread_join(bees[i],NULL);
    }
}
