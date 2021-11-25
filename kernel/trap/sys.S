#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include "../inc/trap.h"

//极端情况下 关中断时发生时钟中断的处理? (未解决)
//解决方案1：进入系统调用前关中断(?是否可行?用户态是否有权限?)
//解决方案2: 核心态不做切换(进入后判断)-->怎么实现?

LEAF(handle_sys)
    .set at
    CLI
    //进入syscall异常处理后引发tlbmiss的情形测试
    mfc0	k0,CP0_EPC

    //清除异常状态 以便能成功进行tlb处理
    la k1, back2here
    mtc0	k1,CP0_EPC
    eret
back2here:
    mtc0	k0,CP0_EPC
    //STI
    //li k0, 0x000003004
    //lw k0, (k0)

    sw		sp, user_sp
    //保存快照 到curtf
    lw sp, curtf
    
    addiu	sp,sp, TF_SIZE //SAVE_TF中还要再减
    SAVE_TF

    lw k0, user_sp
    sw	k0,TF_REG29(sp)
    
    move a0, sp

    //恢复sp
    lw sp, kernel_sp

    nop
    jal SystemCall
    nop

    CLI
    

    
    //恢复快照
    lw sp, curtf
    RESTORE_TF
    //lw sp, tmp_sp  如果是exec 则无需恢复sp 因此需要在将curtf中的sp存储为tmp_sp
    nop

    //更新entryhi(ASID)
    lw k1, curasid
    mtc0	k1,CP0_ENTRYHI

    //将status保存在k1中 下次回到核心态时直接写入
    //li		k1, 0x00007c02
    

    //回到引发中断的下一条指令  （由中断程序完成）
    //(epc + 4) 把正确返回地址存在epc中
    mfc0	k0,CP0_EPC
    move v1, k0
    //sw k0, _cp0_epc_
    //STI  //开中断的瞬间可能会跳到int处理程序中
    //lw k0, _cp0_epc_
    //mtc0	k0,CP0_EPC
    //nop

    CLI
    
    nop
    jr v1
	nop

END(handle_sys)
