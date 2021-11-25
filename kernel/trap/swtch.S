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
    //保存了返回地址ra
    SAVE_TF
    lw	k0, kernel_sp
    sw	k0,TF_REG29(sp)

    //清除时间片并且开中断
    lw sp, kernel_sp
    nop
    jal clear_timer0_int
    nop

    //a1 -> sp
    lw sp, tmp_sp

    //lw	v0,TF_STATUS(sp)				 
    //mtc0	v0,CP0_STATUS
    
    RESTORE_TF
    
    //防止开中断的瞬间跳回

    //mfc0	v1,CP0_EPC
    mfc0	k0,CP0_EPC
    //sw k0, _cp0_epc_
    //nop
    //STI
    //nop
    //lw k0, _cp0_epc_
    //mtc0	k0,CP0_EPC

    //利用指令延迟槽开中断-->可行性?  -->不行
    //li		k1, 0x00007c00
    //mtc0 k1, CP0_STATUS

    //方案：v1用作存储返回地址 -->不适用

    j k0
	nop

END(swtchk2u)