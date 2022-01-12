#include "gpio.h"


#define MAILBOX_BASE           (MMIO_BASE + 0x0000B880)

//mailbox register
#define MAILBOX_REG_READ       ((volatile unsigned int*)(MAILBOX_BASE + 0x00000000 ))
#define MAILBOX_REG_STATUS     ((volatile unsigned int*)(MAILBOX_BASE + 0x00000018 ))
#define MAILBOX_REG_WRITE      ((volatile unsigned int*)(MAILBOX_BASE + 0x00000020 ))

//mailbox status
#define MAILBOX_EMPTY      0x40000000
#define MAILBOX_FULL       0x80000000

//mailbox channel
#define MAILBOX_CH_PPOWER     0
#define MAILBOX_CH_FB         1
#define MAILBOX_CH_V_UART     2
#define MAILBOX_CH_VcHIQ      3
#define MAILBOX_CH_LED        4
#define MAILBOX_CH_BTN        5
#define MAILBOX_CH_TOUCH      6
#define MAILBOX_CH_RESEVRED   7
#define MAILBOX_CH_PROP       8

//operator
#define HARDWARE_BOARD_REVISION  0x00010002
#define HARD_ARN_MEMORY          0x00010005

//tag
#define REQUEST_SUCCEED_TAG     0x80000000
#define REQUEST_FAILED_TAG      0x80000001
#define REQUEST_CODE_TAG        0x00000000
#define END_TAG                 0x00000000

int mailbox_call(unsigned int *);
int mailbox_board_revision();
long mailbox_arm_memory();