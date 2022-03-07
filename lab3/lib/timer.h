
typedef struct slice_timeout {
    struct slice_timeout *next;
    unsigned int timeout;
    char message[64];
}slice_t;

slice_t *timer_header;

void timer_handler();
void timer_init();
void add_timer(unsigned int,char*);
void sort_timer(slice_t *); // short timer first
void updata(unsigned int); // timeout change
void expire(unsigned int);
void timer_info();
void coretime();


extern void core_timer_disable();
extern void core_timer_enable();