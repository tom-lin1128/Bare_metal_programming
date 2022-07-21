#include "mailbox.h"
#include "uart.h"
#include "string.h"

//return 0 fail, return 1 success
int mailbox_call(unsigned char ch,unsigned int *mailbox){

    const int data = ((unsigned int)((unsigned long)mailbox)&~0xF) | (ch & 0xF);//msb 28 bits is data and lsb 4 bits is channel1
    do{asm volatile("nop");}while( *MAILBOX_REG_STATUS & MAILBOX_FULL );
    
    *MAILBOX_REG_WRITE = data;


    while(1){  
        do{asm volatile("nop");}while( *MAILBOX_REG_STATUS & MAILBOX_EMPTY );
        if( *MAILBOX_REG_READ == data )  return 1;
    }
    return 0;
} 

int mailbox_board_revision(){

     unsigned int  mailbox[7];
     mailbox[0] = 7 * 4; // buffer size in bytes
     mailbox[1] = REQUEST_CODE_TAG ; // tags begin
     mailbox[2] = HARDWARE_BOARD_REVISION; // tag identifier
     mailbox[3] = 4; // maximum of request and response value buffer's length.
     mailbox[4] = REQUEST_CODE_TAG;
     mailbox[5] = 0; // value buffer
     mailbox[6] = END_TAG; // tags end
    
     if ( mailbox_call((unsigned char)MAILBOX_CH_PROP,mailbox) )  return mailbox[5];
     else                          return 0;
     
}

long mailbox_arm_memory(){

    unsigned int  mailbox[8];
    mailbox[0] = 8 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE_TAG ; // tags begin
    mailbox[2] = HARD_ARN_MEMORY; // tag identifier
    mailbox[3] = 8; // maximum of request and response value buffer's length.
    mailbox[4] = REQUEST_CODE_TAG;
    mailbox[5] = 0; // value buffer
    mailbox[6] = 0;
    mailbox[7] = END_TAG; // tags end

    if( mailbox_call((unsigned char)MAILBOX_CH_PROP,mailbox) ) return ((mailbox[5] << 32) | mailbox[6]);
    else                        return 0;
}