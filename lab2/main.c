#include "uart.h"
#include "shell.h"

int main(){

	uart_init();

	//uart_puts("Hello world!\n");
	
	shell_s();

    return 0;
}
