
//enable mini uart's interrupt
#define IRQ_BASE            (0x3F00B000)
#define IRQ_BASIC_PENDING   ((volatile unsigned int*)(IRQ_BASE + 0x204))
#define Enable_IRQs1        ((volatile unsigned int*)(IRQ_BASE + 0x210))

#define AUX_INT (1 << 29)



void enable_interrupt();
void disable_interrupt();