#include "timer.h"
#include "allocator.h"
#include "string.h"
void timer_init(){
    timer_header = nullptr;
}

void expire(unsigned int duration){
    unsigned int result;
    asm volatile("mrs %0, cntfrq_el0\n" :"=r"(result));
    asm volatile("msr cntp_tval_el0, %0\n" ::"r"(result * duration));
}

void updata(unsigned int timeout){
    slice_t *tmp = timer_header;
    while(tmp != nullptr){
        tmp->timeout -= timeout;
        tmp = tmp->next;
    }
}

void sort_timer(slice_t *new){
    slice_t *fast = timer_header , *slow = nullptr;
    int f = 0;
    while(fast != nullptr){
        if(fast->timeout > new->timeout && f == 0){
            new->next = fast;
            printf_s("mov pointer\n");
            if(fast == timer_header)
                timer_header = new;
            //if(slow != nullptr)
                slow->next = new;
            f = 1;
        }
        if(fast->next == new && f == 1){
            fast->next = nullptr;
            break;
        }
        slow = fast;
        fast = fast->next;
    }
}

void add_timer(unsigned int after,char* message){
    slice_t *new = (slice_t*)simple_malloc(sizeof(slice_t));
    unsigned int str_len = strlen(message);
    int i;
    new->timeout = after;
    for(i = 0; i < str_len; i++)
        new->message[i] = message[i];
    new->message[i+1] = '\0';
    new->next = nullptr;
    if(timer_header == nullptr){
        timer_header = new;
        expire(after);
        core_timer_enable();
    }
    else{
        slice_t *tmp = timer_header;
        while(tmp->next != nullptr){
            tmp = tmp->next;
        }
        tmp->next = new;
        sort_timer(new);
        expire(timer_header->timeout);
    }

}

void timer_handler(){
    slice_t *tmp = timer_header;
    printf_s(tmp->message);
    printf_c('\n');
    coretime();
    if(tmp->next != nullptr){
        timer_header = timer_header->next;
        updata(tmp->timeout);
        expire(timer_header->timeout);
    }
    else{
        timer_header = nullptr;
        core_timer_disable();
    }
    timer_info();
    printf_s("$ ");
}

void timer_info(){
    slice_t *tmp = timer_header;
    while(tmp != nullptr){
        printf_s(tmp->message);
        printf_c('\n');
        printf_i(tmp->timeout);
        printf_c('\n');
        tmp = tmp->next;
    }
}

void coretime(){
    int count, f;
    int time = 0;
    printf_s("  ");
    asm volatile("mrs  %[result], cntpct_el0": [result]"=r"(count));
    asm volatile("mrs  %[result], cntfrq_el0": [result]"=r"(f));
    time = (count) / f;
    //printf_i(time);
    printf_s("timer interrupt\n");
}
