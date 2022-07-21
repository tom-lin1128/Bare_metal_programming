#include "uart.h"
#include "shell.h"
#include "uartbooting.h"
#include "allocator.h"
#include "thread.h"
#include "systemcall.h"

int main()
{
	uart_init();
	init_allocator();
	// init_thread();
	// schedule();
	// old_kernel_move();
	// Thread_create(fork_test);
	// schedule();
	execl();
	return 0;
}
