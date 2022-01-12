#include "uart.h"


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


}

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

//Display a string
void uart_puts(char *s){

	while(*s){
		uart_send(*s++);
	}
}
