#include "uartbooting.h"
#include "shell.h"

void old_kernel_move(){

    long kernel_start = (long)&_start;
    long kernel_end = (long)&_end;
    printf_h(kernel_end);

    char *base = (char *)OLD_MEMORY_START;

    for (int i = 0; i < kernel_end - kernel_start; i++) 
        *(base + i) = *(char *)(kernel_start + i);

    ((void (*)())((long)execl + base - kernel_start))();
}

void new_kernel_move(){

	int size = 0;
	char *kernel = (char *)NEW_MEMORY_START;
    
    printf_s("Start send the image.\r\n");
	size  = scanf_i();

    printf_s("Image size: ");
    printf_i(size);
    printf_c('\n');
    for (int i = 0; i < size; i++) {
        *(kernel + i) = uart_getc();
    }
    printf_s("End");
    printf_c('\n');

    ((void (*)())(char *)NEW_MEMORY_START)();
}