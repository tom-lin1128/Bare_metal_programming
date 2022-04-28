#include "systemcall.h"
#include "thread.h"
#include "uart.h"
int getpid(){
    int retval;
    asm volatile("mov x8, #0");
    asm volatile("svc 0");
    asm volatile("mov %0, x0" : "=r"(retval));
    return retval;
}
int uart_read(char buf[], int size){
    int retval;
    asm volatile("mov x8, #1");
    asm volatile("svc 0");
    asm volatile("mov %0, x0" : "=r"(retval));
    return retval;
}
int uart_write(const char buf[], int size){
    int retval;
    asm volatile("mov x8, #2");
    asm volatile("svc 0");
    asm volatile("mov %0, x0" : "=r"(retval));
    return retval;
}
int execl(const char* program){
    asm volatile("mov x8, #3");
    asm volatile("svc 0");
}

int fork(){
    int retval;
    asm volatile("mov x9, lr");
    asm volatile("mov x8, #4");
    asm volatile("svc 0");
    asm volatile("mov %0, x0" : "=r"(retval));
    return retval;
}

void exit(){
    asm volatile("mov x8, #5");
    asm volatile("svc 0");
}

int mbox_call(unsigned char ch, unsigned int *mailbox){
    int retval;
    asm volatile("mov x8, #6");
    asm volatile("svc 0");
    asm volatile("mov %0,x0" : "=r"(retval));
}

void kill(int tid){
    asm volatile("mov x8, #7");
    asm volatile("svc 0");
}
