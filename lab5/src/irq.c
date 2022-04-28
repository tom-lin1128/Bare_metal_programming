#include "irq.h"
#include "uart.h"
#include "timer.h"

void enable_interrupt(){
    asm volatile("msr DAIFClr, 0xf");
}

void disable_interrupt(){
    asm("msr DAIFSet, 0xf");
}

void irq_handler(int type, unsigned long sspsr_el1, unsigned long elr_el1, unsigned long esr_el1){

    if(*CORE0_IRQ_SOURCE & 2){
        timer_handler2();
    }
    else if(*IRQ_BASIC_PENDING & AUX_INT){
        uart_interrupt();
    }
    else{
        printf_s("Exception ");
        printf_s("irq_64");
        printf_s(" occur");
        printf_s("  sspsr_el1 : ");
        printf_h(sspsr_el1);
        printf_s(" elr_el1 : ");
        printf_h(elr_el1);
        printf_s(" esr_el1 : ");
        printf_h(esr_el1);
        printf_c('\n');    
    }
}