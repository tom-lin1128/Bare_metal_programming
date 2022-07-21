#include "thread.h"
#include "allocator.h"
#include "timer.h"
#include "shell.h"
#include "cpio.h"
#include "devicetree.h"

extern void switch_to(thread_struct *, thread_struct *);
extern void eload();
int current_tid = 0;
queue runqueue;

void el0(unsigned long long sp_el0, unsigned long long lr)
{
    asm volatile("mov x1, %0" ::"r"(sp_el0));
    asm volatile("msr sp_el0, x1");
    asm volatile("ldr x0, 0");
    asm volatile("msr spsr_el1, x0");
    asm volatile("mov x0, %0" ::"r"(lr));
    asm volatile("msr elr_el1, x0");
    asm volatile("eret");
}

void idel_task()
{
    struct list_head *pos;
    printf_s("idle\n");
    while (1)
    {
        if (!list_empty(&(runqueue.list)))
        {
            list_for_each(pos, &(runqueue.list))
            {
                thread_struct *tmp = list_entry(pos, thread_struct, runqueue_next);
                if (tmp->status == ZOMBIE)
                {
                    list_del(pos);
                    free(tmp);
                }
            }
            schedule();
        }
    }
}

void init_thread()
{
    thread_struct *new_thread = (thread_struct *)dynamic_malloc(sizeof(thread_struct));
    new_thread->context.sp = (unsigned long long int)dynamic_malloc(FRAME_SIZE) + FRAME_SIZE;
    new_thread->context.fp = new_thread->context.sp;
    new_thread->context.lr = (unsigned long long int)&execl;
    new_thread->tid = current_tid++;

    new_thread->usr_sp = (unsigned long long int)dynamic_malloc(FRAME_SIZE) + FRAME_SIZE; // base usr stack point
    new_thread->utask_sp = new_thread->usr_sp;                                            // current usr stack point
    new_thread->kernel_sp = new_thread->context.sp;
    new_thread->elr = (unsigned long long int)&execl;
    asm volatile("mov sp, %0" ::"r"(new_thread->kernel_sp));
    asm volatile("msr tpidr_el1, %0" ::"r"(new_thread));

    new_thread->status = ACTIVE;
    INIT_LIST_HEAD(&(runqueue.list));
    runqueue.idle = new_thread;

    timer_init();
    // core_timer_enable();
    el0(new_thread->usr_sp, &execl);
}
void *Thread_create(void (*func)())
{
    thread_struct *new_thread = dynamic_malloc(sizeof(thread_struct));
    new_thread->context.sp = (unsigned long long int)dynamic_malloc(FRAME_SIZE) + FRAME_SIZE;
    new_thread->context.fp = new_thread->context.sp;
    new_thread->context.lr = (unsigned long long int)func;
    new_thread->tid = current_tid++;

    new_thread->usr_sp = (unsigned long long int)dynamic_malloc(FRAME_SIZE) + FRAME_SIZE;
    new_thread->kernel_sp = new_thread->context.sp;
    new_thread->utask_sp = new_thread->usr_sp;
    // new_thread->elr = (unsigned long long  int)func;

    new_thread->status = ACTIVE;
    list_add_tail(&(new_thread->runqueue_next), &(runqueue.list));
    return new_thread;
}

thread_struct *get_current()
{
    unsigned long long tmp;
    asm volatile("mrs %0, tpidr_el1"
                 : "=r"(tmp));
    thread_struct *cur_task = (thread_struct *)tmp;
    return cur_task;
}

void context_switch(thread_struct *next)
{
    thread_struct *pre = get_current();
    int sp;
    asm volatile("mrs %0, sp_el0"
                 : "=r"(sp));
    pre->utask_sp = sp;
    asm volatile("msr sp_el0, %0" ::"r"(next->utask_sp));

    switch_to(pre, next);
}

void schedule()
{
    thread_struct *next;
    struct list_head *pos;
    if (list_empty(&(runqueue.list)))
    {
        thread_struct *idle = runqueue.idle;
        context_switch(idle);
    }
    else
    {
        list_for_each(pos, &(runqueue.list))
        {
            thread_struct *tmp = list_entry(pos, thread_struct, runqueue_next);
            if (tmp->status == ZOMBIE)
            {
                free((long long unsigned int *)tmp->usr_sp);
                free((long long unsigned int *)tmp->kernel_sp);
                free(tmp->context.lr);
                list_del(pos);
                free(tmp);
            }
            if (list_empty(&(runqueue.list)))
            {
                thread_struct *idle = runqueue.idle;
                context_switch(idle);
            }
        }
        list_for_each(pos, &(runqueue.list))
        {
            next = list_entry(pos, thread_struct, runqueue_next);
            break;
        }
        list_del(pos);
        list_add_tail(pos, &(runqueue.list));
        context_switch(next);
    }
}

void do_exec(const char *program)
{
    int size, start_addr;
    start_addr = (int)cpio_exec(program, get_deviceprop("linux,initrd-start"), &size);
    int *text_sec = (int *)dynamic_malloc(size);

    for (int i = 0; i <= size; i++)
        *((char *)text_sec + i) = *((char *)start_addr + i);

    thread_struct *task = get_current();
    task->context.lr = &eload;
    printf_h(task->kernel_sp);
    printf_c('\n');
    task->context.sp = task->kernel_sp;
    task->context.fp = task->kernel_sp;
    task->utask_sp = task->usr_sp;
    // task->elr = text_sec;
    asm volatile("msr tpidr_el1, %0" ::"r"(task));
    asm volatile("mov sp, %0" ::"r"(task->kernel_sp));
    asm volatile("msr sp_el0, %0" ::"r"(task->utask_sp));
    asm volatile("msr elr_el1, %0" ::"r"(text_sec));
    asm volatile("eret");
}

void do_fork(unsigned long long sp)
{
    thread_struct *cur_thread = get_current();
    thread_struct *new_thread = Thread_create(&eload);
    int sp_el0;
    asm volatile("mrs %0, sp_el0"
                 : "=r"(sp_el0));
    cur_thread->utask_sp = sp_el0;

    for (int i = 0; i < 10; i++)
        new_thread->context.x[i] = cur_thread->context.x[i];

    long long size = (cur_thread->kernel_sp - sp);

    new_thread->context.sp = new_thread->kernel_sp - size;

    for (int i = 0; i < size; ++i)
        *((char *)new_thread->kernel_sp - i) = *((char *)cur_thread->kernel_sp - i);

    size = cur_thread->usr_sp - cur_thread->utask_sp;
    new_thread->utask_sp = new_thread->usr_sp - size;

    for (int i = 0; i < size; ++i)
        *((char *)new_thread->usr_sp - i) = *((char *)cur_thread->usr_sp - i);

    trap_frame *child = (trap_frame *)new_thread->context.sp;
    trap_frame *parent = (trap_frame *)sp;
    child->x0 = 0;
    parent->x0 = new_thread->tid;
}

void do_kill(int tid)
{
    struct list_head *pos;
    list_for_each(pos, &(runqueue.list))
    {
        thread_struct *tmp = list_entry(pos, thread_struct, runqueue_next);
        if (tmp->tid == tid)
        {
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

void thread_info()
{
    struct list_head *pos;
    int ssp;
    thread_struct *idle = runqueue.idle;
    printf_i(idle->tid);
    printf_c(' ');
    printf_h(idle->kernel_sp);
    printf_c(' ');
    printf_h(idle->usr_sp);
    printf_c(' ');
    printf_h(idle->utask_sp);
    printf_c(' ');
    list_for_each(pos, &(runqueue.list))
    {
        thread_struct *tmp = list_entry(pos, thread_struct, runqueue_next);
        printf_i(tmp->tid);
        printf_c(' ');
        printf_h(tmp->kernel_sp);
        printf_c(' ');
        printf_h(tmp->usr_sp);
        printf_c(' ');
        printf_h(tmp->utask_sp);
        printf_c(' ');
        /*printf_h(tmp->usr_sp);
        printf_c(' ');
        printf_h(tmp->kernel_sp);
        printf_c(' ');
        asm volatile("mov %0, sp":"=r"(ssp));
        printf_h(ssp);
        printf_c(' ');*/
    }
    printf_c('\n');
}
