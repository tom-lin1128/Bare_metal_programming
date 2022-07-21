int getpid();
int uart_read(char[],int);
int uart_write(const char[],int);
int exec(const char*);
int fork();
void exit();
int mbox_call(unsigned char, unsigned int *);
void kill(int);