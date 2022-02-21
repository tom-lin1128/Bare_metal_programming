#include "uart.h"
#include "shell.h"
#include "uartbooting.h"
#include "allocator.h"

int main(){

	uart_init();
	init_allocator();
	//old_kernel_move();
	exec();
    return 0;
}
