.set nomips16
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include "../inc/trap.h"

//从分发入口到处理程序不会使用栈和普通寄存器 故可以当作直接跳到这里来处理
//保存现场 (从异常分发程序过来) 
//将当前寄存器状态保存到curtf
//加载

.set noreorder

LEAF(handle_int)
    .set at
    CLI
    mfc0 k0, CP0_EPC
    sw sp, tmp_sp

    lw sp, cur_context
    
    addiu	sp,sp, TF_SIZE //SAVE_TF中还要再减
    SAVE_TF
    lw k0, tmp_sp
    sw	k0,TF_REG29(sp)

    lw	v0,TF_STATUS(sp)

    //恢复核心sp
    //注意这个位置是有隐患的 若当前处于内核态 kernel_sp并非最新位置
    //因此需要判断进入int的位置是否为核心

    lw sp, tmp_sp
    //-----------特殊情形检测----------------//
    //--epc属于内核空间
    mfc0 k0, CP0_EPC
    li k1, 0x80000000
    and k0, k0, k1
    bnez k0, j_from_kernel
    //--------------------------------------//
    lw sp, kernel_sp
j_from_kernel:
    sw sp, kernel_sp
    
    //计数器清零
    nop
    jal clear_timer0_int
    nop

    nop
    //jal disable_timer0
    nop

    nop
    jal timer_work
    nop

    //lw sp, cur_context
    //RESTORE_TF
    //eret
    //nop


    //恢复调度器 scheduler_tf_ptr
    lw sp, scheduler_tf_ptr
    RESTORE_TF
    mtc0 ra, CP0_EPC
    mfc0 k0, CP0_EPC
    nop
    eret
	nop

END(handle_int)


LEAF(set_exl)
    mfc0 v0, CP0_STATUS
    li k0, 0xFFFFFFFD
    and v0, k0
    mtc0 v0, CP0_STATUS
    ehb
    jr ra
    nop
END(set_exl)