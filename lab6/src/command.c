#include "command.h"
#include "uart.h"
#include "mailbox.h"
#include "devicetree.h"
#include "string.h" 
#include "cpio.h"
#include "allocator.h"
#include "exception.h"
#include "systemcall.h"

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

void command_el0(){
    extern void from_el1_to_el0();
    from_el1_to_el0();
}

void command_svc(){
    system_call_sim();
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
void command_forks(){
    /*int ret = fork();
    if(ret == 0){
        printf_s("child\n");
        exit();
    }
    else{
        printf_s("parent\n");
    }*/
    if(fork() == 0){
        while(1);
    }
    else{
        shell_s();
    }
}
void command_fork(){
    printf_s("\nFork Test, pid ");
	printf_i(getpid());
	printf_c('\n');
	int cnt = 1;
	int ret = 0;
    /*if((ret = fork()) == 0){
        exec("syscall.img");
    }
    else{
        printf_s("parent\n");
    }*/
	if((ret = fork()) == 0){
		long long cur_sp;
        asm volatile("mov %0, sp" : "=r"(cur_sp));
        printf_s("first child pid: ");
        printf_i(getpid());
        printf_s(", cnt: ");
        printf_i(cnt);
        printf_s(", ptr: ");
        printf_h(&cnt);
        printf_s(", sp: ");
        printf_h(cur_sp);
        printf_c('\n');
        //cnt++;
        if((ret = fork()) != 0){
            asm volatile("mov %0, sp" : "=r"(cur_sp));
            printf_s("first child pid: ");
            printf_i(getpid());
            printf_s(", cnt: ");
            printf_i(cnt);
            printf_s(", ptr: ");
            printf_h(&cnt);
            printf_s(", sp: ");
            printf_h(cur_sp);
            printf_c('\n');
        }
        else{
            while(cnt < 5){
                asm volatile("mov %0, sp" : "=r"(cur_sp));
                printf_s("second child pid: ");
                printf_i(getpid());
                printf_s(", cnt: ");
                printf_i(cnt);
                printf_s(", ptr: ");
                printf_h(&cnt);
                printf_s(", sp: ");
                printf_h(cur_sp);
                printf_c('\n');
                for(int i = 0; i < 10000000; i++);
                ++cnt;
            }
        }
        exit();
	}
    else{
        printf_s("parent here, pid: ");
        printf_i(getpid());
        printf_s(", child ");
        printf_i(ret);
        printf_c('\n');
    }


}

void command_allocate(){
    printf_s("--------------------------\n");
    printf_s("      buddy system test   \n");
    printf_s("--------------------------\n");
    /*int *a = (int *)memory_alloc(4096);
    int *b = (int *)memory_alloc(8192);
    int *c = (int *)memory_alloc(4066);

    printf_h((long)a);
    printf_c('\n');
    printf_h((long)b);
    printf_c('\n');
    printf_h((long)c);
    printf_c('\n');

    memory_free(a);
    memory_free(b);
    memory_free(c);*/

    printf_s("\n\n");

    printf_s("--------------------------\n");
    printf_s("  dynamic allocate test   \n");
    printf_s("--------------------------\n");

    int *d = dynamic_malloc(FRAME_SIZE);
    int *e = dynamic_malloc(FRAME_SIZE);
    int *f = dynamic_malloc(FRAME_SIZE);
    int *g = dynamic_malloc(FRAME_SIZE);

    printf_h((long)d);
    printf_c('\n');
    printf_h((long)e);
    printf_c('\n');
    printf_h((long)f);
    printf_c('\n');
    printf_h((long)g);
    printf_c('\n');

    free(d);
    free(e);
    free(f);
    free(g);
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