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
        /*printf_s("interrupt ");
        printf_s("irq_64");
        printf_s(" occur\n");
        printf_s("  exception type : ");
        switch(esr_el1>>26) {
            case 0b000000: printf_s("Unknown"); break;
            case 0b000001: printf_s("Trapped WFI/WFE"); break;
            case 0b001110: printf_s("Illegal execution"); break;
            case 0b010101: printf_s("System call"); break;
            case 0b100000: printf_s("Instruction abort, lower EL"); break;
            case 0b100001: printf_s("Instruction abort, same EL"); break;
            case 0b100010: printf_s("Instruction alignment fault"); break;
            case 0b100100: printf_s("Data abort, lower EL"); break;
            case 0b100101: printf_s("Data abort, same EL"); break;
            case 0b100110: printf_s("Stack alignment fault"); break;
            case 0b101100: printf_s("Floating point"); break;
            default: printf_s("Unknown"); break;
        }
        printf_c('\n');*/
        timer_handler();
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