.text
.global _start
_start:
	/*
	 * Vector table
	 */ 
	b reset
	b .
	b .
	b .
	b .
	b .
	b irq_handler
	b .

reset:
	/*
	 * Set vector address in CP15 VBAR register
	 */ 
	ldr	r0, =_start
	mcr	p15, 0, r0, c12, c0, 0	@Set VBAR

	/*
	 * Set the cpu to SVC32 mode, Disable FIQ/IRQ
	 */  
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr ,r0

	/*
	 * Defines access permissions for each coprocessor
	 */  
    mov	r0, #0xfffffff
    mcr	p15, 0, r0, c1, c0, 2  	

	/*
	 * Invalidate L1 I/D                                                                                                                   
	 */
	mov	r0, #0					@Set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@Invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@Invalidate icache
	
	/*
	 * Set the FPEXC EN bit to enable the FPU
	 */ 
	mov r3, #0x40000000
	fmxr FPEXC, r3
	
	/*
	 * Disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000		@Clear bits 13 (--V-)
	bic	r0, r0, #0x00000007		@Clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00001000		@Set bit 12 (---I) Icache
	orr	r0, r0, #0x00000002		@Set bit 1 (--A-) Align
	orr	r0, r0, #0x00000800		@Set bit 11 (Z---) BTB
	mcr	p15, 0, r0, c1, c0, 0

	/*
	 * Initialize stacks                                                                                                                  
	 */
init_stack:     
	/*svc mode stack*/
	msr cpsr, #0xd3
	ldr sp, _stack_svc_end

	/*undef mode stack*/
	msr cpsr, #0xdb
	ldr sp, _stack_und_end

	/*abort mode stack*/	
	msr cpsr,#0xd7
	ldr sp,_stack_abt_end

	/*irq mode stack*/	
	msr cpsr,#0xd2
	ldr sp, _stack_irq_end
	
	/*fiq mode stack*/
	msr cpsr,#0xd1
	ldr sp, _stack_fiq_end
	
	/*user mode stack, enable FIQ/IRQ*/
	msr cpsr,#0x10
	ldr sp, _stack_usr_end

	/*Call main*/
	b main

irq_handler:
	//因为产生IRQ异常后自动保存LR中的地址是被IRQ打断指令的下下指令的地址，需人为进行修改
	sub lr, lr, #4
	//因为IRQ模式下使用的R0-R12寄存器和user模式使用的是同一组，处理异常之前现将其进行压栈保护
	stmfd sp!, {r0-r12, lr}
	//跳转到异常处理
	bl do_irq
	//异常返回，将R0-R12中的值出栈，使其恢复到异常之前的状态
	//将SPSR中的状态恢复给CPSR
	//将栈中保存的LR寄存器的值出栈给PC，使其跳转到被异常打断的点继续执行
	ldmfd sp!, {r0-r12, pc}^

_stack_svc_end:      
	.word stack_svc + 512
_stack_und_end:      
	.word stack_und + 512
_stack_abt_end:      
	.word stack_abt + 512
_stack_irq_end:      
    .word stack_irq + 512
_stack_fiq_end:
    .word stack_fiq + 512
_stack_usr_end:      
    .word stack_usr + 512

.data
stack_svc:      
	.space 512
stack_und:
	.space 512
stack_abt:      
	.space 512
stack_irq:      
	.space 512
stack_fiq:      
	.space 512
stack_usr:      
	.space 512
