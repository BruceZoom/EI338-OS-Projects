#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

static struct node *list = NULL;

void _add(struct task *t, struct node **target){
    insert(target, t);
}

// add a task to the list 
void add(char *name, int priority, int burst){
    struct task *t = malloc(sizeof(struct task));
	t->name = name;
    t->priority = priority;
    t->burst = burst;
    _add(t, &list);
}

void reverse(struct node *temp){
    _add(temp->task, &list);
    free(temp);
}

void execute(struct node *temp){
    run(temp->task, temp->task->burst);
    free(temp->task);
    free(temp);
}

// invoke the scheduler
void schedule(){
    struct node *remain = list->next;
    list->next = NULL;
    traverse(remain, reverse);
    traverse(list, execute);
}
