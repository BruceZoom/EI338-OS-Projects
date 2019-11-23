#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LENGTH 64
#define SPACE(node) ((node)->hi - (node)->lo + 1)

typedef struct MEM_BLOCK_T{
    int lo;
    int hi;
    char name[NAME_LENGTH];
    struct MEM_BLOCK_T *next;
}mem_block_t;

mem_block_t *head;

void init(int size){
    head = (mem_block_t*)malloc(sizeof(mem_block_t));
    mem_block_t *node = (mem_block_t*)malloc(sizeof(mem_block_t));
    node->lo = 0;
    node->hi = size-1;
    strcpy(node->name, "Unused");
    node->next = NULL;
    head->next = node;
}

void delete_next(mem_block_t **node){
    mem_block_t *tmp;
    tmp = (*node)->next;
    (*node)->next = tmp->next;
    free(tmp);
}

void free_list(){
    while (head->next != NULL)
    {
        delete_next(&head);
    }
    free(head);
}

void status(){
    mem_block_t *iter = head->next;
    while (iter != NULL)
    {
        if(iter->next == NULL && iter->name[0] == 'U'){
            printf("Addresses [%d] . . .\n", iter->lo);
            break;
        }
        else{
            printf("Addresses [%d:%d] %s\n", iter->lo, iter->hi, iter->name);
            iter = iter->next;
        }
    }
}

// request memory
// return 0 if succeed, otherwise return -1
int request(char name[NAME_LENGTH], int block, char mode){
    mem_block_t *iter;
    // target->next is the space to insert
    mem_block_t *target;
    switch (mode)
    {
    // worst fit
    case 'W':
        target = NULL;
        iter = head;
        while (iter->next != NULL)
        {
            if(iter->next->name[0] == 'U' && SPACE(iter->next) >= block
             && (target == NULL || SPACE(iter->next) > SPACE(target->next))){
                target = iter;
            }
            iter = iter->next;
        }
        break;
    // best fit
    case 'B':
        target = NULL;
        iter = head;
        while (iter->next != NULL)
        {
            if(iter->next->name[0] == 'U' && SPACE(iter->next) >= block
             && (target == NULL || SPACE(iter->next) < SPACE(target->next))){
                target = iter;
            }
            iter = iter->next;
        }
        break;
    // first fit
    case 'F':
        target = head;
        while(target->next != NULL && (target->next->name[0] != 'U' || SPACE(target->next) < block))
            target = target->next;
        break;
    default:
        printf("Unknown flag\n");
        return -1;
    }
    // failed
    if(target == NULL || target->next == NULL){
        printf("Insufficient memory\n");
        return -1;
    }
    else{
        // succeed and just fit
        if(SPACE(target->next) == block){
            strcpy(target->next->name, "Process ");  
            strcat(target->next->name, name);
        }
        // succeed but need to split
        else{
            iter = (mem_block_t*) malloc(sizeof(mem_block_t));
            iter->lo = target->next->lo;
            iter->hi = iter->lo + block - 1;
            target->next->lo += block;
            strcpy(iter->name, "Process ");
            strcat(iter->name, name);
            iter->next = target->next;
            target->next = iter;
        }
        return 0;
    }
}

// release memory
// return 0 if succeed, otherwise return -1
int release(char id[64]){
    char name[64] = "Process ";
    mem_block_t *iter = head;
    strcat(name, id);
    while(iter->next != NULL){
        if(!strcmp(iter->next->name, name)){
            int flag = 1;
            // merge backward
            if(iter->next->next != NULL && iter->next->next->name[0] == 'U'){
                iter->next->next->lo = iter->next->lo;
                delete_next(&iter);
                flag = 0;
            }
            // merge forward
            if(iter != head && iter->name[0] == 'U'){
                iter->hi = iter->next->hi;
                delete_next(&iter);
                flag = 0;
            }
            // rename
            if(flag){
                strcpy(iter->next->name, "Unused");
            }
            return 0;
        }
        iter = iter->next;
    }
    printf("Process %s is not found\n", id);
}

// compact
void comapct(){
    int offset = 0;
    mem_block_t *iter = head;
    while (iter->next != NULL)
    {
        if(iter->next->name[0] == 'U')
        {
            // update last hole
            if(iter->next->next == NULL){
                iter->next->lo -= offset;
                break;
            }
            // delete hole
            else{
                offset += SPACE(iter->next);
                delete_next(&iter);
            }
        }
        // update address
        else
        {
            iter = iter->next;
            iter->lo -= offset;
            iter->hi -= offset;
        }
    }
}

int main(int argc, char **args){
    int size = 0;
    char com[64];
    char flag[4];
    if(argc > 1){
        size = atoi(args[1]);
    }
    init(size);
    while (1)
    {
        scanf("%s", com);
        if(com[0] == 'X') break;
        switch (com[1])
        {
        // compact
        case '\0':
            comapct();
            break;
        // release
        case 'L':
            scanf("%s", com);
            release(com);
            break;
        // request
        case 'Q':
            scanf("%s%d%s", com, &size, flag);
            request(com, size, flag[0]);
            break;
        // status
        default:
            status();
            break;
        }
    }
    return 0;
}
