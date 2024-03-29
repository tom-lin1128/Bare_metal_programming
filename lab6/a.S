.section ".text.boot"

.global _start
.global from_el1_to_el0

#define PA(p) (p - 0xffff000000000000)

_start:
	//load devicetree
	ldr     x2, =_devicetree
    str     x0, [x2]
  	//read cpu id, stop slave core
  	mrs x1, mpidr_el1
  	and x1, x1, #3
  	cbz x1, 2f
  	//cpu id > 0, stop
1:  wfe
  	   b 1b

2:	// cpu id == 0
	// set top of stack(stack grows to lower address)
	ldr	x1, =PA(_start)
    mov	sp,	x1

	//clear bss
	ldr	x1, =PA(_bss_start)
	ldr	w2, =_bss_size
	
3:	cbnz	w2,	from_el2_to_el1
	str xzr, [x1], #8
	sub	w2, w2, #1
	cbnz w2, 3b

mmu_start:
	bl identity_mapping_init
	b 4f


from_el2_to_el1:
    ldr x1, =PA(_start)
    msr sp_el1, x1
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
	// set exception table
	bl  set_exception_vector_table
    mov x0, 0x5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
	adr x0, mmu_start
    msr elr_el2, x0
    eret // return to EL1

from_el1_to_el0:
    ldr x1, =_start
    msr sp_el0, x1
    //mov x0, #0x3c0
    //msr spsr_el1, x0 // save program status register
	ldr x0, 0
    msr spsr_el1, x0
    adr x0, 4f
    msr elr_el1, x0  // Exception Linker Register
    eret

//jump to c code, sould not return
timers:
	//bl core_timer_enable
	msr DAIFClr, 0xf


	
/*4:	bl	main
	// hait this core
	b	1b*/

4: ldr x9, =vkernel_start
   br  x9

vkernel_start: bl main