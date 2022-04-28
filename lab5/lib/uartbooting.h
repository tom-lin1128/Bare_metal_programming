#define NEW_MEMORY_START  (volatile unsigned int*)(0x80000)
#define OLD_MEMORY_START  (volatile unsigned int*)(0x60000)

#include "uart.h"

extern volatile unsigned int _start;
extern volatile unsigned int _end;

void old_kernel_move();
void new_kernel_move();