#include "uart.h"
#include "exception.h"
extern void syscall_interrupt();
extern void core_timer_handler();
extern void core_timer_enable();

char* exception_type[]={
    "synchronous_sp_el0",
    "irq_sp_el0",
    "fiq_sp_el0",
    "serror_sp_el0",
    "synchronous_sp_elx",
    "irq_sp_elx",
    "fiq_sp_elx",
    "serror_sp_elx",
    "synchronous_aarch64",
    "irq_aarch64",
    "fiq_aarch64",
    "serror_aarch64",
    "synchronous_aarch32",
    "irq_aarch32",
    "fiq_aarch32",
    "serror_aarch32",
};

void system_call_sim(){
    syscall_interrupt();
}

void exception_info(int x0,int x1,int x2,int x3){
    printf_s("interrupt ");
    printf_s(exception_type[x0]);
    printf_s(" occur");
    printf_s("  sspsr_el1 : ");
    printf_h(x1);
    printf_s(" elr_el1 : ");
    printf_h(x2);
    printf_s(" esr_el1 : ");
    printf_h(x3);
    printf_c('\n');
}

void exception_handler(int type, unsigned long sspsr_el1, unsigned long elr_el1, unsigned long esr_el1){
    if(type == 9){
        printf_s("interrupt ");
        printf_s(exception_type[type]);
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
        printf_c('\n');
        asm volatile("mrs     x0, cntfrq_el0\n"
                     "lsl     x0, x0, #1\n"
                     "msr     cntp_tval_el0, x0\n");
        coretime();
        asm volatile("mov x0, 0\n""msr cntp_ctl_el0, x0\n");//disable timer interrupt
    }
    else{
        printf_s("Exception ");
        printf_s(exception_type[type]);
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

void coretime(){
    int count, f;
    int time = 0;
    printf_s("  ");
    asm volatile("mrs  %[result], cntpct_el0": [result]"=r"(count));
    asm volatile("mrs  %[result], cntfrq_el0": [result]"=r"(f));
    time = (count) / f;
    printf_i(time);
    printf_s(" second timer interrupt\n");
}
