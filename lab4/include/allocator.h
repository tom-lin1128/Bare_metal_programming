#include "uart.h"
#include "buddy.h"
extern volatile unsigned int _bss_end;
extern volatile unsigned int _end;

 
typedef struct memory_chunck{
    int isused;
    int index;
    long start_address;
    struct memory_chunck *next;
}CHUNK;

typedef struct allocated_frame{
    long start_address;
    int allocate_size;
    int isfull;
    struct memory_chunck *first;
    struct allocated_frame *next;
}allocated_frame_t;

void  init_allocator();
void* fix_malloc(int size);
void* dynamic_malloc(int size); 
void  free(int *address);


//simple allocator
void* malloc();