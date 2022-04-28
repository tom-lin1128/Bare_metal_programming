#include "thread.h"
#include "allocator.h"
#include "timer.h"
#include "shell.h"
#include "cpio.h"
#include "devicetree.h"

extern void switch_to(thread_struct *,thread_struct *);
int current_tid = 0;
queue runqueue;

void el0(unsigned long long sp_el0,unsigned long long lr){
	asm volatile("mov x1, %0"::"r"(sp_el0));
	asm volatile("msr sp_el0, x1");
	asm volatile("ldr x0, 0");
	asm volatile("msr spsr_el1, x0");
	asm volatile("mov x0, %0"::"r"(lr));
	asm volatile("msr elr_el1, x0");
	asm volatile("eret");
}

void idel_task(){
    struct list_head *pos;
    printf_s("idle\n");
    while(1){
        if(!list_empty(&(runqueue.list))){
            list_for_each(pos,&(runqueue.list)){
                thread_struct *tmp = list_entry(pos,thread_struct,runqueue_next);
                if(tmp->status == ZOMBIE){
                    list_del(pos);
                    free(tmp);
                }
            }
            schedule();
        }
    }
}
void init_thread(){
    thread_struct* new_thread = (thread_struct*)dynamic_malloc(sizeof(thread_struct));
    new_thread->context.sp = (unsigned long long int)dynamic_malloc(FRAME_SIZE) + FRAME_SIZE;
    new_thread->context.fp = new_thread->context.sp;
    new_thread->context.lr = (unsigned long long  int)&exec;
    new_thread->tid = current_tid++;

    new_thread->usr_sp = (unsigned long long int)dynamic_malloc(FRAME_SIZE) + FRAME_SIZE;
    new_thread->kernel_sp = new_thread->context.sp;

    new_thread->status = ACTIVE;
    INIT_LIST_HEAD(&(runqueue.list));
    runqueue.idle = new_thread;
    asm volatile("msr tpidr_el1, %0"::"r"(new_thread));
    asm volatile("mov sp, %0"::"r"(new_thread->kernel_sp));
    el0(new_thread->usr_sp ,&exec);
    //printf_h(new_thread->context.sp);
    //printf_c('\n');
    //thread_info();
    //list_add_tail(&(new_thread->runqueue_next),&(runqueue.list));
    //expire(1);
    //core_timer_enable();
    //schedule();
}
void* Thread_create(void (*func)()){
    thread_struct* new_thread = dynamic_malloc(sizeof(thread_struct));
    new_thread->context.sp = (unsigned long long int)dynamic_malloc(FRAME_SIZE) + FRAME_SIZE;
    new_thread->context.fp = new_thread->context.sp;
    new_thread->context.lr = (unsigned long long  int)&func;
    new_thread->tid = current_tid++;

    new_thread->usr_sp = (unsigned long long int)dynamic_malloc(FRAME_SIZE) + FRAME_SIZE;
    new_thread->kernel_sp = new_thread->context.sp;

    new_thread->status = ACTIVE;
    list_add_tail(&(new_thread->runqueue_next),&(runqueue.list));
    //printf_h(new_thread->usr_sp);
    //printf_c('\n');
    //thread_info();
    return new_thread;
}
thread_struct* get_current() {
    unsigned long long  tmp;
    asm volatile("mrs %0, tpidr_el1" : "=r"(tmp));
    thread_struct* cur_task = (thread_struct*)tmp;
    return cur_task;
}

void context_switch(thread_struct* next){
    thread_struct *pre = get_current();
    switch_to(pre,next);
}

void schedule(){
    thread_struct* next;
    struct list_head* pos;
    if(list_empty(&(runqueue.list))){
        thread_struct *idle = runqueue.idle;
        context_switch(idle);
    }
    else{
        list_for_each(pos,&(runqueue.list)){
            thread_struct *tmp = list_entry(pos,thread_struct,runqueue_next);
            if(tmp->status == ZOMBIE){
                free((long long unsigned int *)tmp->usr_sp);
                printf_c('\n');
                list_del(pos);
                free(tmp);
            }
        }
        list_for_each(pos,&(runqueue.list)){
            next = list_entry(pos, thread_struct, runqueue_next);
            break;
        }
        list_del(pos);
        list_add_tail(pos,&(runqueue.list));
        context_switch(next);
    }
}

void do_execl(const char *program){
    thread_struct *task = get_current();
    task->context.lr = cpio_exec(program,get_deviceprop("linux,initrd-start"));
    free(task->usr_sp);
    task->usr_sp = (unsigned long long)dynamic_malloc(FRAME_SIZE);
    task->context.sp = task->usr_sp;
    task->context.fp = task->usr_sp;
}

void do_fork(unsigned long long lr,unsigned long long sp){
    thread_struct *cur_thread = get_current();
    thread_struct *new_thread = Thread_create(lr);
    int sp_el0;
    asm volatile("mrs %0, sp_el0":"=r"(sp_el0));
    cur_thread->utask_sp = sp_el0;
    printf_h(cur_thread->usr_sp);
    printf_c('\n');
    cur_thread->context.sp = sp;
    for(int i = 0; i < 10; i++)
        new_thread->context.x[i] = cur_thread->context.x[i];
    //cur_thread->context.sp = sp;
    long long size = (cur_thread->kernel_sp - cur_thread->context.sp );
    printf_h(sp);
    printf_c('\n');
    printf_h(cur_thread->kernel_sp);
    printf_c('\n');
    printf_h(cur_thread->context.sp);
    printf_c('\n');
    printf_i(size);
    printf_c('\n');
    new_thread->context.sp = new_thread->kernel_sp - size;
    for(int i = 0; i < size; ++i)
        *((char *)new_thread->kernel_sp - i) = *((char *)cur_thread->kernel_sp - i);
    size = cur_thread->usr_sp - cur_thread->utask_sp;
    for(int i = 0; i < size; ++i)
        *((char *)new_thread->usr_sp - i) = *((char *)cur_thread->usr_sp - i);

    trap_frame *child = (trap_frame *)new_thread->context.sp;
    trap_frame *parent = (trap_frame *)cur_thread->context.sp;
    child->x0 = 0;
    printf_h(new_thread->context.sp);
    printf_c('\n');
    parent->x0 = new_thread->tid;
    printf_h(cur_thread->context.sp);
    printf_c('\n');
}

void do_kill(int tid){
    struct list_head* pos;
    list_for_each(pos,&(runqueue.list)){
        thread_struct *tmp = list_entry(pos,thread_struct,runqueue_next);
        if(tmp->tid == tid){
            list_del(pos);
            free(tmp->usr_sp);
            free(tmp->kernel_sp);
            free(tmp);
            printf_s("kill thread ");
            printf_i(tid);
            printf_c('\n');
        }
    }
}






void thread_info(){
    struct list_head *pos;  
    list_for_each(pos,&(runqueue.list)){
        thread_struct *tmp = list_entry(pos,thread_struct,runqueue_next);
        printf_i(tmp->tid);
        printf_c(' ');
        printf_h(tmp->context.sp);
        printf_c(' ');
    }
    printf_c('\n');
}

