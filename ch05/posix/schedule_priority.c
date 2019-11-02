#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

static struct node *list = NULL;
static struct task *prior_task;

// add a task to the list 
void add(char *name, int priority, int burst){
    struct task *t = malloc(sizeof(struct task));
	t->name = name;
    t->priority = priority;
    t->burst = burst;
    insert(&list, t);
}

void reverse(struct node *temp){
    insert(&list, temp->task);
    free(temp);
}

void find_next_prior(struct node *temp){
    if(prior_task == NULL || prior_task->priority < temp->task->priority){
        prior_task = temp->task;
    }
}

// invoke the scheduler
void schedule(){
    struct node *remain = list->next;
    list->next = NULL;
    traverse(remain, reverse);
    while(list != NULL){
        prior_task = NULL;
        traverse(list, find_next_prior);
        run(prior_task, prior_task->burst);
        delete(&list, prior_task);
        free(prior_task);
    }
}
