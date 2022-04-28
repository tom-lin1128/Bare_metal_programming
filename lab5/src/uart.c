#include "uart.h"
#include "queue.h"

QUEUE_T *readbuf,*writebuf;
void uart_init(){
	unsigned int reg;

	//initialize uart
	*AUX_ENABLE = *AUX_ENABLE | 1; // enable mini UART
	*AUX_MU_CNTL = 0; // Disable transmitter and receiver during config
	*AUX_MU_IER = 0; // Disable interrupt
	*AUX_MU_LCR = 3; // set the data size to 8 bit
	*AUX_MU_MCR = 0; // Don't need auto flow control
	*AUX_MU_BAUD = 270; // 115200 baud rate
	*AUX_MU_IIR = 6; //no FIFO

	//map uart1 to gpio pins
	reg = *GPFSEL1;
	reg &= !((7<<12)|(7<<15)); // address of gpio14,gpio15
	reg |= (2<<12)|(2<<15); // set to alt5
	*GPFSEL1 = reg;
	*GPPUD = 0; // enable pins 14 and 15
	reg = 150;
	while(reg --){ asm volatile("nop");}
	*GPPUDCLK0 = (1<<14)|(1<<15);
	reg = 150;
	while(reg--) { asm volatile("nop");}
	*GPPUDCLK0 = 0;	//flush GPIO setup
	*AUX_MU_CNTL = 3; // Enable tx and rx

	//readbuf  = queue_new();
	//printf_h(readbuf);
	//printf_c('\n');
	//writebuf = queue_new();
	//printf_h(writebuf);
	//enable_uart_interrupt();
	//*AUX_MU_IER = 1;
}



void uart_interrupt(){
	disable_uart_interrupt();
	//tx
	//Transmit holding register empty
	if(*AUX_MU_IIR & 0x2){
		while(!queue_is_empty(writebuf)){
			while(!(*AUX_MU_LSR & 0x20)){
				asm volatile("nop");
			}
			*AUX_MU_IO = dequeue(writebuf);
		}
		//printf_c('t');
		disable_tx_interrupt();
	}
	//rx
	else if(*AUX_MU_IIR & 0x4){
		while(*AUX_MU_LSR & 0x01){
			char r = (char)(*AUX_MU_IO);
			enqueue(readbuf,r);
		}
		//printf_s("rx");
	}
	enable_uart_interrupt();
}





/*
//asyn send
void uart_send(unsigned int c){
	enqueue(writebuf,(char)c);
	enable_tx_interrupt();
}


//asyn getc
char uart_getc(){
	do {asm volatile("nop");} while (!(*AUX_MU_LSR & 0x01));

	char data = (char)dequeue(readbuf);
	//data = (char)(*AUX_MU_IO);
	return data == '\r' ? '\n' : data;
}
*/





//write data
void uart_send(unsigned int c){
	//wait until we can send
	do{asm volatile("nop");}while(!(*AUX_MU_LSR & 0x20));

	*AUX_MU_IO = c;
	if(c == '\n'){
		do{asm volatile("nop");}while(!(*AUX_MU_LSR & 0x20));
		*AUX_MU_IO = '\r';
	}
}


//Receive a char
char uart_getc(){
	char data;
	do{asm volatile("nop");}while(!(*AUX_MU_LSR & 0x01));
	data = (char)(*AUX_MU_IO);
	return data == '\r' ? '\n' : data;
}





char uart_getb() {
    char r;
    /* wait until something is in the buffer */
    do {
        asm volatile("nop");
    } while (*AUX_MU_LSR & 0x01);
    /* read it and return */
    r = (char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return r;
}

int check_digit(char ch) { return (ch >= '0') && (ch <= '9'); }



//Display a string
void uart_puts(char *s){

	while(*s){
		uart_send(*s++);
	}
}

void printf_s(char *ch) { uart_puts(ch); }
void printf_c(char ch) { uart_send(ch); }

void printf_i(int x) {
    if (x < 0) {
        printf_c('-');
        x = -x;
    }
    if (x >= 10) printf_i(x / 10);
    printf_c(x % 10 + '0');
}

int scanf_i() {
    int x = 0, f = 0;
    char ch = 0;
    while (!check_digit(ch)) {
        f |= ch == '-';
        ch = uart_getc();
    }
    while (check_digit(ch)) {
		x = (x << 3) + (x << 1) + (ch ^ 48);
		ch = uart_getc();
	}
    return f ? -x : x;
}

void printf_h(unsigned int d) {
    unsigned int n;
    int c;
    for (c = 28; c >= 0; c -= 4) {
        // get highest tetrad
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9 ? 0x37 : 0x30;
        uart_send(n);
    }
}


void enable_uart_interrupt(){
	*Enable_IRQs1 = AUX_INT;
}

void disable_uart_interrupt(){
	*Disable_IRQs1 = AUX_INT;
}

void enable_tx_interrupt(){
	*AUX_MU_IER |= 2; 
}

void disable_tx_interrupt(){
	*AUX_MU_IER &= (~2); 
}

void enable_rx_interrupt(){
	*AUX_MU_IER |= 1; 
}

void disable_rx_interrupt(){
	*AUX_MU_IER &= (~1); 
}
