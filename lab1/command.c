#include "command.h"
#include "uart.h"
#include "mailbox.h"
#include "string.h" 

void command_help(){
    uart_puts("\n");
    uart_puts("\tCommand:\n");
    uart_puts("\thelp:\t\tprint this help menu\n");
    uart_puts("\thello:\t\tprint \"Hello World!\"\n");
    uart_puts("\tboard_info:\tprint board revision\n");
    uart_puts("\tmem_info:\tprint arm memory detail\n");
    uart_puts("\treboot:\t\treboot the device\n");
    uart_puts("\n");
}

void command_hello(){
    uart_puts("Hello world!\n");
}

void command_not_found(char *s){
    uart_puts("Error: command ");
    uart_puts(s);
    uart_puts(" not found, try type help\n");
}

void command_board_revision(){

    char str[20];

    int board_revision = mailbox_board_revision();
    if(board_revision){
        stritohex(board_revision, sizeof(int), str);
        uart_puts("Board Revision : ");
        uart_puts(str);
        uart_puts("\n");
    }
    else  uart_puts("Unable to access board revision\n");
}

void command_arm_memory(){
    long arm_base_addr = mailbox_arm_memory();
    if( arm_base_addr ){
        char str[20];
        uart_puts("Arm memory data\n");

        uart_puts("- Base Address : ");
        stritohex((int)(arm_base_addr >> 32), sizeof(int), str);
        uart_puts(str);
        uart_send('\n');

        uart_puts("- Size :         ");
        stritohex((int)(arm_base_addr & 0xffffffff ), sizeof(int), str);
        uart_puts(str);
        uart_send('\n');
    }
    else{
        uart_puts("Unable to access arm memory\n");
    }

}

void command_reboot(){
    uart_puts("Start Rebooting...\n");
    reset(10);
    while(1);
}

void set(long addr, unsigned int value) {
    volatile unsigned int* point = (unsigned int*)addr;
    *point = value;
}

void reset(int tick) {                 // reboot after watchdog timer expire
    set(PM_RSTC, PM_PASSWORD | 0x20);  // full reset
    set(PM_WDOG, PM_PASSWORD | tick);  // number of watchdog tick
}