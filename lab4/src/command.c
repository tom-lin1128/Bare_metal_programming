#include "command.h"
#include "uart.h"
#include "mailbox.h"
#include "devicetree.h"
#include "string.h" 
#include "cpio.h"
#include "allocator.h"

void command_help(){
    uart_puts("\n");
    uart_puts("\tCommand:\n");
    uart_puts("\thelp:\t\tprint this help menu\n");
    uart_puts("\thello:\t\tprint \"Hello World!\"\n");
    uart_puts("\tboard_info:\tprint board revision\n");
    uart_puts("\tmem_info:\tprint arm memory detail\n");
    uart_puts("\tls:\t\tprint all file in dir\n");
    uart_puts("\tcat:\t\tprint file content\n");
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

void command_cpio_ls(){
    cpio_ls(get_deviceprop("linux,initrd-start"));
}

void command_cpio_cat(){
    char file_name[100];
    char input;
    int counter = 0;

    printf_s("Filename: ");
    while(1){
        input = uart_getc();
        if((input == 127) && counter > 0){
            if(counter){
                counter--;
                printf_s("\rFilename: ");
                for(int i = 0; i < counter; i++){
                    printf_c(file_name[i]);
                }
                printf_c(' ');
                printf_s("\rFilename: ");
                for(int i = 0; i < counter; i++){
                    printf_c(file_name[i]);
                }
		    }
        }
        else if((input == 10) || (input == 13)){
            file_name[counter] = '\0';
            printf_c(input);
            break;
        }
        else{
            file_name[counter] = input;
            counter ++;
            uart_send(input);
        }
    }
    cpio_print_file_content(file_name,get_deviceprop("linux,initrd-start"));

}

void command_devicetree_info(){
    parse_devicetree();
    printf_c('\n');
}


void command_allocate(){
    printf_s("--------------------------\n");
    printf_s("      buddy system test   \n");
    printf_s("--------------------------\n");
    int *a = (int *)memory_alloc(4096);
    int *b = (int *)memory_alloc(8192);
    int *c = (int *)memory_alloc(10000);


    memory_free(a);
    memory_free(b);
    memory_free(c);

    printf_s("\n\n");

    printf_s("--------------------------\n");
    printf_s("  dynamic allocate test   \n");
    printf_s("--------------------------\n");

    int *d = dynamic_malloc(17);
    int *e = dynamic_malloc(68);
    int *f = dynamic_malloc(8);

    printf_h(d);
    printf_c('\n');
    printf_h(e);
    printf_c('\n');
    printf_h(f);
    printf_c('\n');

    free(d);
    free(e);
    free(f);
    printf_c('\n');
    printf_s("test end\n");
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