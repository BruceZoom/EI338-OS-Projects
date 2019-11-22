#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];
/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

void print_vector(int v[NUMBER_OF_RESOURCES])
{
    int i;
    for(i=0; i<NUMBER_OF_RESOURCES; i++)
        printf("%d ", v[i]);
    printf("\n");
}

void print_matrix(int m[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES])
{
    int i, j;
    for(i=0; i<NUMBER_OF_CUSTOMERS; i++)
    {
        for(j=0; j<NUMBER_OF_RESOURCES; j++)
            printf("%d ", m[i][j]);
        printf("\n");
    }
}

void v_cpy(int *dst, int *src){
    int i;
    for(i=0; i<NUMBER_OF_RESOURCES; i++)
        dst[i] = src[i];
}

void v_add(int *dst, int *src, int sgn){
    int i;
    for(i=0; i<NUMBER_OF_RESOURCES; i++)
        dst[i] += src[i] * sgn;
}

int v_less(int *left, int *right){
    int i;
    int flag = 0;
    for(i=0; i<NUMBER_OF_RESOURCES; i++)
        flag += (left[i] <= right[i]);
    return (flag == NUMBER_OF_RESOURCES);
}

int safety(){
    int work[NUMBER_OF_RESOURCES];
    int finished = 0;
    int i, flag = 1;
    v_cpy(work, available);
    do{
        if(!flag) return -1;
        flag = 0;
        for(i=0; i<NUMBER_OF_CUSTOMERS; i++){
            if(v_less(need[i], work)){
                flag = 1;
                v_add(work, allocation[i], 1);
                finished = (finished | (1 << i));
            }
        }
    }while(finished != (1<<(NUMBER_OF_CUSTOMERS)) - 1);
    return 1;
}

void _release_resources(int customer_num, int release[], int sgn){
    v_add(available, release, sgn);
    v_add(allocation[customer_num], release, -sgn);
    v_add(need[customer_num], release, sgn);
}

int request_resources(int customer_num, int request[]){
    int flag;
    if(!v_less(request, need[customer_num]))
    {
        printf("request exceeds need\n");
        return -1;
    }
    if(!v_less(request, available))
    {
        printf("request denied due to limited resource\n");
        return -1;
    }
    _release_resources(customer_num, request, -1);
    if(safety()) return 0;
    _release_resources(customer_num, request, 1);
    printf("request denied due to deadlock\n");
    return -1;
}

void release_resources(int customer_num, int release[]){
    _release_resources(customer_num, release, 1);
}

int main(int argc, char **args){
    FILE *file;
    int i, j;
    int request[NUMBER_OF_RESOURCES];
    char s[] = "%d\0\0";
    file = fopen("maximum.txt", "r");
    for(i=0; i<NUMBER_OF_CUSTOMERS; i++){
        for(j=0; j<NUMBER_OF_RESOURCES; j++){
            fscanf(file, ((j==NUMBER_OF_RESOURCES-1)?"%d\n":"%d,"), &maximum[i][j]);
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
            // printf("%d ", need[i][j]);
        }
    }
    fclose(file);
    for(j=0; j<NUMBER_OF_RESOURCES; j++)
        available[j] = ((j<argc-1)?atoi(args[j+1]):0);
    while(1){
        scanf("%s", s);
        if(s[0] == '*'){
            printf("available:\n");
            print_vector(available);
            printf("maximum:\n");
            print_matrix(maximum);
            printf("allocation:\n");
            print_matrix(allocation);
            printf("need:\n");
            print_matrix(need);
        }
        else{
            scanf("%d", &j);
            for(i=0; i<NUMBER_OF_RESOURCES; i++)
                scanf("%d", &request[i]);
            if(s[1] == 'Q' || s[1] == 'q'){
                if(request_resources(j, request) == 0){
                    printf("request satisfied\n");
                }
            }
            else if(s[1] == 'L' || s[1] == 'l'){
                if(v_less(request, allocation[j])){
                    release_resources(j, request);
                    printf("resources released\n");
                }
                else{
                    printf("release exceeds allocation\n");
                }
            }
            else{
                printf("unkown command\n");
            }
        }
    }
    return 0;
}
