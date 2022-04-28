#include "gpio.h"

#define MEM_LOW  ( (long)((volatile unsigned int *)(0x100000)))
#define MEM_HIGH MMIO_BASE
#define FRAME_SIZE (1 << 12)
#define FRAME_MAX_NUMBER 8

#define PHYSICAL_MEMORY (1 << 19)
#define PHYSICAL_PAGES  (PHYSICAL_MEMORY / FRAME_SIZE)

#define nullptr ((void *)0)

typedef struct buddy_struct{
    int id;
    long address;
    int number_of_frame;
    int used;
    struct buddy_struct *left;
    struct buddy_struct *right;
    struct buddy_struct *sibling;
    struct buddy_struct *parent;
}BUDDY;

void buddy_init();
void *memory_alloc(int size);
void memory_free(void *addr);

void pop_from_mem_list(int, BUDDY*);
void push_into_mem_list(int, BUDDY*);