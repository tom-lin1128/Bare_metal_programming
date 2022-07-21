#include "signal.h"
#include "allocator.h"
#include "systemcall.h"

void init_signal(signal_t *new){
    new->sig_sp = (int)dynamic_malloc(FRAME_SIZE) + FRAME_SIZE;
    new->signal_handler = (int)&exit;
    new->signal = 0;
    new->mask = 0;
}

void run_signal(void (*handler)(), int sig_sp){
    asm volatile("msr elr_el1, %0" : : "r"(handler));
    asm volatile("msr sp_el0,  %0" : : "r"(sig_sp));
    asm volatile("mov x0, #0");
    asm volatile("msr spsr_el1, x0");
    asm volatile("mov x30, %0" : : "r"(&sigreturn));
    asm volatile("eret");
}

void copy_signal(signal_t *p, signal_t *t){
    p->signal_handler = t->signal_handler;
}

void return_from_signal(int sp0){
    asm volatile("mov sp, %0" : : "r"(sp0));
}


void sigreturn(){
    asm volatile("mov x8, 10");
    asm volatile("svc 0");
}

void signal(int SIG,void (*function)()){
    asm volatile("mov x8, #8");
    asm volatile("svc 0");
}

void skill(int tid, int SIG){
    asm volatile("mov x8, #9");
    asm volatile("svc 0");
}