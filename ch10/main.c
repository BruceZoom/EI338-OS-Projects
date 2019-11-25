#include <stdio.h>
#include <stdlib.h>

#define PAGE_MASK 65280
#define OFFSET_MASK 255

#define get_page(adr) (((adr) & PAGE_MASK)>>8)
#define get_offset(adr) ((adr) & OFFSET_MASK)

#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define NUM_OF_FRAME 128
#define MEMORY_SIZE (NUM_OF_FRAME * FRAME_SIZE)
#define TLB_SIZE 16

int page_table[PAGE_TABLE_SIZE];
char memory[MEMORY_SIZE];
int valid_bit[PAGE_TABLE_SIZE];
int free_frame_index;

int TLB_page[TLB_SIZE];
int TLB_frame[TLB_SIZE];
int next_TLB;
int in_tlb[PAGE_TABLE_SIZE];

FILE *backing_store;

int tlb_hits;
int page_faults;

typedef struct NODE
{
    int page;
    struct NODE *pred;
    struct NODE *succ;
} Node;

Node *head;
Node *tail;

void init(){
    int i;
    free_frame_index = 0;
    for(i=0; i<TLB_SIZE; i++){
        TLB_page[i] = -1;
        TLB_frame[i] = -1;
    }
    for(i=0; i<PAGE_TABLE_SIZE; i++){
        valid_bit[i] = 0;
        in_tlb[i] = 0;
    }
    next_TLB = 0;

    tlb_hits = 0;
    page_faults = 0;

    head = (Node*)malloc(sizeof(Node));
    tail = (Node*)malloc(sizeof(Node));
    head->pred = NULL;
    head->succ = tail;
    tail->pred = head;
    tail->succ = NULL;
}

// get frame number from LTB
int TLB_get_frame(int page){
    int i;
    for(i=0; i<TLB_SIZE; i++){
        if(TLB_page[i] == -1) return -1;
        if(TLB_page[i] == page) return TLB_frame[i];
    }
    return -1;
}

// update TLB
void update_TLB(int page, int frame){
    int tmp = TLB_page[next_TLB];
    if(tmp > 0) in_tlb[tmp] = 0;
    TLB_page[next_TLB] = page;
    TLB_frame[next_TLB] = frame;
    next_TLB = (next_TLB + 1) % TLB_SIZE;
    in_tlb[page] = 1;
}

void insert(Node **node){
    tail->pred->succ = *node;
    (*node)->pred = tail->pred;
    (*node)->succ = tail;
    tail->pred = *node;
}

int get_victim_frame(){
    int tmp;
    Node *node = head->succ;
    while(node != tail && (!valid_bit[node->page] || in_tlb[node->page])) node = node->succ;
    node->pred->succ = node->succ;
    node->succ->pred = node->pred;
    tmp = node->page;
    free(node);
    valid_bit[tmp] = 0;
    return page_table[tmp];
}

void clear(){
    Node *node = head->succ;
    while (node != NULL)
    {
        free(node->pred);
        node = node->succ;
    }
    free(tail);
}

void swap_in_frame(int page, int frame){
    fseek(backing_store, page * FRAME_SIZE, 0);
    fread(memory + frame * FRAME_SIZE, FRAME_SIZE, 1, backing_store);
}

// get frame number
int get_frame(int page){
    int frame = TLB_get_frame(page);
    // TLB miss
    if(frame < 0){
        tlb_hits--;
        // the page is valid
        if(valid_bit[page]){
            frame = page_table[page];
        }
        // the page is not valid: page-fault
        else
        {
            page_faults++;
            // free frame available
            if(free_frame_index < NUM_OF_FRAME){
                frame = free_frame_index;
                page_table[page] = frame;
                free_frame_index++;
            }
            // no free frame, need victim
            else{
                frame = get_victim_frame();
            }
            Node *node = (Node*)malloc(sizeof(Node));
            node->page = page;
            insert(&node);
            valid_bit[page] = 1;
            swap_in_frame(page, frame);
        }
        update_TLB(page, frame);
    }
    return frame;
}

int main(int argc, char **args){
    FILE *infile;
    // FILE *outfile;
    int adr;
    int cnt = 0;

    if(argc < 2) return 0;
    infile = fopen(args[1], "r");
    backing_store = fopen("BACKING_STORE.bin", "rb");
    init();

    while (fscanf(infile, "%d", &adr) != EOF)
    {
        // get physical address
        int page = get_page(adr);
        int frame = get_frame(page);
        int offset = get_offset(adr);
        printf("Virtual Address: %d ", adr);
        adr = (frame << 8) + offset;
        // retrieve data
        printf("Physical Address: %d Value: %d\n", adr, memory[adr]);
        cnt++;
    }

    printf("Statistics:\n");
    printf("TLB hit rate: %d / %d\n", cnt+tlb_hits, cnt);
    printf("Page fault rate: %d / %d\n", page_faults, cnt);

    clear();
    fclose(backing_store);
    fclose(infile);
}
