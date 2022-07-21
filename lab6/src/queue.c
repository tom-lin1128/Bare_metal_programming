#include "queue.h"
#include "allocator.h"

QUEUE_T *queue_new(void){
    QUEUE_T *p = (QUEUE_T *)simple_malloc(sizeof(QUEUE_T));
    p->head = 0;
    p->tail = 0;
    return p;
}

int queue_is_empty(QUEUE_T *self){
    if(self->head % MAX_QUEUE_LEN == self->tail % MAX_QUEUE_LEN)
        return 1;
    return 0;
}

int queue_is_full(QUEUE_T *self){
    if((self->tail+1) % MAX_QUEUE_LEN == self->head)
        return 1;
    return 0;
}

void enqueue(QUEUE_T *self, char data){
    if(queue_is_full(self)){
        return;
    }
    else{
        self->buffer[self->tail] = data;
        self->tail++;
        self->tail %= MAX_QUEUE_LEN;
    }
    return;
}

char dequeue(QUEUE_T *self){
    if(!queue_is_empty(self)){
        char tmp = self->buffer[self->head];
        self->head++;
        self->head %= MAX_QUEUE_LEN;
        return tmp;
    }
    return '\0';
}