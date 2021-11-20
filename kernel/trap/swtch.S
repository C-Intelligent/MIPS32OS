#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include "../inc/trap.h"

//void  swtch(struct trapframe* save_to, struct trapframe * to_load);
LEAF(swtchk2u)
    .set at

    sw		sp, kernel_sp

    //临时保存a1
    sw a1, tmp_sp

    //a0 -> sp
    move sp, a0
    addiu	sp,sp, TF_SIZE //SAVE_TF中还要再减
    SAVE_TF

    //a1 -> sp
    lw sp, tmp_sp
    RESTORE_TF
    
    //跳转到用户空间运行
    //lw k0, (ra)
    //mtc0	ra,CP0_EPC
    //nop
    //nop
    //j ra
    //nop

    nop
    j	ra
	//rfe
	nop

END(swtchk2u)