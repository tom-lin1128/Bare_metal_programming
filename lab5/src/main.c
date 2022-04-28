#include "uart.h"
#include "shell.h"
#include "uartbooting.h"
#include "allocator.h"
#include "thread.h"
#include "systemcall.h"

void foo2(){
	printf_s("test program!!!!!\n");
}
void fork_test(){
	printf_s("\nFork Test, pid ");
	printf_i(getpid());
	printf_c('\n');
	int cnt = 1;
	int ret = 0;
	if((ret = fork()) == 0){
		printf_i(ret);
		printf_s(" child\n");
		exit();
	}
	else{
		printf_i(ret);
		printf_s(" parent\n");
		exit();
	}
}
int main(){
	uart_init();
	init_allocator();
	init_thread();
	fork_test();
	//old_kernel_move();
    //Thread_create(fork_test);
	//schedule();
	//idel_task();
	//command_el0();
	//exec();
    return 0;
}
