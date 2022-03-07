#include "allocator.h"
long new_addr = (long)((volatile unsigned int *)(0x400000));

void* simple_malloc(int size){
    char *addr_start = (char *)new_addr;
    addr_start += size;
    new_addr += size;
    return addr_start;
}