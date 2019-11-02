#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

static struct node *list = NULL;
static struct node *stack = NULL;
static struct task *prior_task = NULL;

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

void find_prior_inv(struct node *temp){
    if(prior_task == NULL || prior_task->priority > temp->task->priority)
    {
        prior_task = temp->task;
    }
}

void execute(struct node *temp){
    if(temp->task->burst > QUANTUM){
        run(temp->task, QUANTUM);
        temp->task->burst -= QUANTUM;
        insert(&stack, temp->task);
    }
    else{
        run(temp->task, temp->task->burst);
        free(temp->task);
    }
    free(temp);
}

// invoke the scheduler
void schedule(){
    struct node *head;
    int last_priority = 0;
    stack = list;
    list = NULL;
    while(stack != NULL){
        prior_task = NULL;
        traverse(stack, find_prior_inv);
        delete(&stack, prior_task);
        insert(&list, prior_task);
    }
    stack = NULL;
    while(list != NULL){
        head = list;
        if(head->task->priority < last_priority && stack != NULL){
            traverse(stack, reverse);
            stack = NULL;            
        }
        else{
            list = list->next;
            last_priority = head->task->priority;
            execute(head);
        }
        if(list == NULL){
            traverse(stack, reverse);
            stack = NULL;
        }
    }
}
