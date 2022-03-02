#include "irq.h"

void enable_interrupt(){
    asm volatile("msr DAIFClr, 0xf");
}

void disable_interrupt(){
    asm("msr DAIFSet, 0xf");
}