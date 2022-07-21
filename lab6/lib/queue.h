#define MAX_QUEUE_LEN 1024
typedef struct queue_t{
    int head;
    int tail;
    char buffer[MAX_QUEUE_LEN];
} QUEUE_T;


QUEUE_T *queue_new(void);
//void queue_delete(void *self);
int queue_is_empty(QUEUE_T *self);
int queue_is_full(QUEUE_T *self);
//int queue_peek(const NODE_T *self);
void enqueue(QUEUE_T *self, char data);
char dequeue(QUEUE_T *self);