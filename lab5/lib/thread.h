#include "list.h"
typedef enum {
    ACTIVE,
    INACTIVE,
    WAIT,    
    ZOMBIE,
} THREAD_STATUS;

typedef struct{
    unsigned long long x0;
    unsigned long long x1;
    unsigned long long x2;
    unsigned long long x3;
    unsigned long long x4;
    unsigned long long x5;
    unsigned long long x6;
    unsigned long long x7;
    unsigned long long x8;
    unsigned long long x9;
    unsigned long long x10;
    unsigned long long x11;
    unsigned long long x12;
    unsigned long long x13;
    unsigned long long x14;
    unsigned long long x15;
    unsigned long long x16;
    unsigned long long x17;
    unsigned long long x18;
    unsigned long long x19;
    unsigned long long x20;
    unsigned long long x21;
    unsigned long long x22;
    unsigned long long x23;
    unsigned long long x24;
    unsigned long long x25;
    unsigned long long x26;
    unsigned long long x27;
    unsigned long long x28;
    unsigned long long x29;
    unsigned long long x30;
}trap_frame;

typedef struct{
    unsigned long long x[10];
    unsigned long long fp;
    unsigned long long lr;
    unsigned long long sp;
}context_d;

typedef struct{
    context_d context;
    int tid;
    THREAD_STATUS status;
    unsigned long long utask_sp;
    unsigned long long usr_sp;
    unsigned long long kernel_sp;
    unsigned long long elr;
    trap_frame *tf;

    struct list_head runqueue_next;
}thread_struct;

typedef struct{
    thread_struct *idle;
    struct list_head list;
}queue;

void idel_task();
void* Thread_create(void (*)());
void init_thread();
void thread_info();
thread_struct* get_current(); 
void schedule();
void context_switch(thread_struct*);
void do_exec(const char *);
void do_fork(unsigned long long);
void do_kill(int tid);