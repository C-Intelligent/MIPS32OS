#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include <trap.h>




	
.macro RESTORE_ALL							 
		RESTORE_SOME								 
		lw	sp,TF_REG29(sp)  /* Deallocate stack */  
.endm

.set	noreorder
.macro RESTORE_ALL_AND_RET					 
		RESTORE_SOME							 
		lw	k0,TF_EPC(sp) 				 
		lw	sp,TF_REG29(sp)  /* Deallocate stack */  
		jr	k0 								 
		rfe 								 
.endm


.macro get_sp
	mfc0	k1, CP0_CAUSE
	andi	k1, 0x107C
	xori	k1, 0x1000
	bnez	k1, 1f
	nop
	li	sp, 0x82000000
	j	2f
	nop
1:
	bltz	sp, 2f
	nop
	lw	sp, KERNEL_SP
	nop

2:	nop


.endm
