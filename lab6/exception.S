.section ".text"
// load general registers from stack
.macro load_all
    ldp x30, x0, [sp, 16 * 15]
    msr elr_el1, x0
    ldp x0, x1, [sp, 16 * 16]
    msr spsr_el1, x0 
    //msr sp_el0, x1 
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    add sp, sp, 16 * 17
.endm

// save general registers to stack
.macro save_all
    sub sp, sp, 16 * 17
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    mrs x0, elr_el1
    stp x30, x0, [sp, 16 * 15]
    mrs x0, spsr_el1
    //mrs x1, sp_el0
    stp x0, x1, [sp ,16 * 16]
.endm



exception_handl:
    bl get_exception_vector_table
    bl exception_info
    load_all
    eret


.align 11 // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:

  b synchronous_sp_el0 // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b irq_sp_el0
  .align 7
  b fiq_sp_el0
  .align 7
  b serror_sp_el0
  .align 7

  b synchronous_sp_elx
  .align 7
  b irq_sp_elx
  .align 7
  b fiq_sp_elx
  .align 7
  b serror_sp_elx
  .align 7

  b synchronous_aarch64
  .align 7
  b irq_aarch64
  .align 7
  b fiq_aarch64
  .align 7
  b serror_aarch64
  .align 7

  b synchronous_aarch32
  .align 7
  b irq_aarch32
  .align 7
  b fiq_aarch32
  .align 7
  b serror_aarch32
  .align 7


.global set_exception_vector_table
set_exception_vector_table:
    adr x0, exception_vector_table
    msr vbar_el1, x0
    ret


.global get_exception_vector_table
get_exception_vector_table:
    mrs x1, spsr_el1
    mrs x2, elr_el1
    mrs x3, esr_el1
    ret


.global syscall_interrupt
syscall_interrupt:
    svc 1
    ret

systemcall:
    mrs x0, esr_el1
    mrs x1, elr_el1
    mov x2, sp
    adr x3, eload
    bl exception_handler_systemcall


.global eload
eload: 
   load_all
   //msr DAIFClr, 0xf
   eret










synchronous_sp_el0:
    save_all
    mov x0, #0
    bl exception_handl       
    ret

irq_sp_el0:
    save_all
    mov x0, #1
    bl exception_handl 
    ret

fiq_sp_el0:
    save_all
    mov x0, #2
    bl exception_handl 
    ret

serror_sp_el0:
    save_all
    mov x0, #3
    bl exception_handl 
    ret

synchronous_sp_elx:
    save_all
    bl systemcall
    

irq_sp_elx:
    save_all
    mov x0, #5
    //bl get_exception_vector_table
    bl irq_handler
    load_all
    eret

fiq_sp_elx:
    save_all
    mov x0, #6
    bl exception_handl 
    ret

serror_sp_elx:
    save_all
    mov x0, #7
    bl exception_handl 
    ret

synchronous_aarch64:
    save_all
    bl systemcall


irq_aarch64:
    save_all
    mov x0, #9
    //bl get_exception_vector_table
    bl irq_handler
    load_all
    eret

fiq_aarch64:
    save_all
    mov x0, #10
    bl exception_handl 
    ret

serror_aarch64:
    save_all
    mov x0, #11
    bl exception_handl 
    ret

synchronous_aarch32:
    save_all
    mov x0, #12
    bl exception_handl 
    ret

irq_aarch32:
    save_all
    mov x0, #13
    bl exception_handl 
    ret

fiq_aarch32:
    save_all
    mov x0, #14
    bl exception_handl 
    ret

serror_aarch32:
    save_all
    mov x0, #15
    bl exception_handl 
    ret