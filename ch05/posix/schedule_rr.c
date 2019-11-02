#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

static struct node *list = NULL;
static struct node *stack = NULL;

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
    stack = list->next;
    list->next = NULL;
    traverse(stack, reverse);
    stack = NULL;
    while(list != NULL){
        head = list;
        list = list->next;
        execute(head);
        if(list == NULL){
		    list = NULL;
            traverse(stack, reverse);
            stack = NULL;
        }
    }
}
