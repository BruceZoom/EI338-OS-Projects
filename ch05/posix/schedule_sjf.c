#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

static struct node *list = NULL;
static struct task *min_task;

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

void find_next_min(struct node *temp){
    if(min_task == NULL || min_task->burst > temp->task->burst){
        min_task = temp->task;
    }
}

// invoke the scheduler
void schedule(){
    struct node *remain = list->next;
    list->next = NULL;
    traverse(remain, reverse);
    while(list != NULL){
        min_task = NULL;
        traverse(list, find_next_min);
        run(min_task, min_task->burst);
        delete(&list, min_task);
        free(min_task);
    }
}
