#include "shell.h"
#include "command.h"
#include "uart.h"
#include "string.h"
#include "uartbooting.h"
#include "allocator.h"

long kernel_start = (long)&_start;

void shell_s(){
	
	char input;
	int counter = 0;
	char buffer[MAX_LENGTH];
	Control P_input;
 
  for(int i = 0; i < MAX_LENGTH; i++){
		buffer[i] = ' ';
	}

	uart_puts("$ ");

	while(1){
		input = uart_getc();
		P_input = Parse(input);
		list_command(P_input,input,buffer,&counter);
	}
}


Control Parse(char c){
	if((c > 128 && c < 0)) return unknowns;

	if(c == back_space_1)	return back_space_1;
	else if(c == back_space_2)	return back_space_2;
	else if(c == line_feed) return ENTER_line;
	else return INPUT;
}


void list_command(Control P, char input, char *buffer, int *counter){
	if(P == unknowns) return ;
	
	else if(P == back_space_1 || P == back_space_2){
		
		if((*counter)){
			(*counter)--;
			uart_puts("\r$ ");
			for(int i = 0; i < *counter; i++){
				uart_send(buffer[i]);
			}
			uart_send(' ');
			uart_puts("\r$ ");
			for(int i = 0; i < *counter; i++){
				uart_send(buffer[i]);
			}
		}
	}
	else if(P == ENTER_line){
 
		uart_send(input);

		buffer[(*counter)] = '\0';


		if	    (strcmp("help",  buffer   ) )  	command_help();
		else if (strcmp("hello", buffer   ) )  	command_hello();
		else if (strcmp("reboot",buffer   ) ) 	command_reboot();
   		else if (strcmp("board_info",buffer) )  command_board_revision();
		else if (strcmp("mem_info", buffer ) )	command_arm_memory();
		else if (strcmp("lm", 		buffer ) )	((void (*)())((long)new_kernel_move - (long)kernel_start + (long)OLD_MEMORY_START))();
		else if (strcmp("addr",     buffer ) )	{
			printf_h((long)new_kernel_move);
			printf_c('\n');
			printf_h((long)new_kernel_move + (long)OLD_MEMORY_START - (long)kernel_start);
			printf_c('\n');
		}
		else if (strcmp("ls",        buffer ) )  command_cpio_ls();
		else if (strcmp("cat",		 buffer ) )	 command_cpio_cat();
		else if (strcmp("d",		 buffer ) )  command_devicetree_info();
		else if (strcmp("a",         buffer ) ){
			char* a = simple_malloc(100);
			printf_h(a);
			printf_c('\n');
		}
		else if (*(counter) == 0)				*(counter) = 0;
		else 								    command_not_found(buffer);
    
		(*counter) = 0;
		for(int i = 0; i < MAX_LENGTH; i++){
			  buffer[i] = ' ';
		}

		//new line head
		uart_puts("$ ");	
	}
	else if(P == INPUT){

		uart_send(input);

		buffer[*counter] = input;
		(*counter)++;
	}
 
}

void exec(){
	while(1){
		shell_s();
	}
}