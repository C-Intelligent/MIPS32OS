#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include "../inc/trap.h"

LEAF(handle_sys)
    .set at
    //临时保存sp
    sw sp, tmp_sp
    //保存快照 到curtf
    lw sp, curtf
    
    addiu	sp,sp, TF_SIZE //SAVE_TF中还要再减
    SAVE_TF

    lw k0, tmp_sp
    sw	k0,TF_REG29(sp)
    
    CLI

    move a0, sp

    //恢复sp
    lw sp, tmp_sp

    nop
    jal SystemCall
    nop

    STI
    //恢复快照
    lw sp, curtf
    RESTORE_TF
    //lw sp, tmp_sp  如果是exec 则无需恢复sp 因此需要在将curtf中的sp存储为tmp_sp
    nop

    //更新entryhi(ASID)
    //mfc0	k0,CP0_ENTRYHI
    //entryHi |= asid
	//li k1, 0xffffff00
    //and k0,k0, k1
    lw k1, curasid
 	//or	k0,k0, k1
    mtc0	k1,CP0_ENTRYHI

    //debug
    //mfc0	k1,CP0_EPC
    //mtc0	ra,CP0_EPC

    //lw s0, (ra)
    //nop
    //jal	ra
    //nop

    nop
    eret
    //j	ra
	//rfe
	nop

END(handle_sys)
