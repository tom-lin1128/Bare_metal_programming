#include "allocator.h"

long new_addr = (long)&_bss_end;
long end_addr = (long)&_end;

void* simple_malloc(int size){
    if(end_addr > new_addr){
        char *addr_start = (char *)new_addr;
        addr_start += size;
        new_addr += size;
        return addr_start;
    }else{
        printf_s("no memory space!!!");
        printf_c('\n');
        return ;
    }
}