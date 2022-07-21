
#define SIGKILL 9
typedef struct{
    int signal;
    int mask;
    int sig_sp;
    int signal_handler;
}signal_t;

void init_signal(signal_t *);
void run_signal(void(*)(), int);
void copy_signal(signal_t *, signal_t *);
void return_from_signal(int);

//syscall
void signal(int, void (*)());
void skill(int ,int);
void sigreturn();