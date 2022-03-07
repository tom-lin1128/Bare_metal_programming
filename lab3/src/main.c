#include "uart.h"
#include "shell.h"
#include "uartbooting.h"
#include "allocator.h"

int main(){

	uart_init();
	//old_kernel_move();
	printf_s("welcome\n");
	exec();
    return 0;
}
