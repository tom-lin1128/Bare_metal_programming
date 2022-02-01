.section ".text"

.global _start

_start:
	//load devicetree
	ldr     x2, =_devicetree
    str     x0, [x2]
  	//read cpu id, stop slave core
  	mrs x1, mpidr_el1
  	and x1, x1, #3
  	cbz x1, setup
  	//cpu id > 0, stop
halt:  wfe
  	   b halt
setup:	// cpu id == 0
	
	//set top of stack(stack grows to lower address) 
	ldr	x1, =_start
	mov	sp,	x1

	//clear bss
	ldr	x1, =_bss_start
	ldr	w2, =_bss_size
3:	cbz	w2,	4f
	str xzr, [x1], #8
	sub	w2, w2, #1
	cbnz w2, 3b

	//jump to c code, sould not return
4:	bl	main
	// hait this core
	b	halt
