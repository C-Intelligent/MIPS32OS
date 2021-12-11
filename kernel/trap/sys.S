#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include "../inc/trap.h"

//极端情况下 关中断时发生时钟中断的处理? (未解决)
//解决方案1：进入系统调用前关中断(?是否可行?用户态是否有权限?)
//解决方案2: 核心态不做切换(进入后判断)-->怎么实现?

.set noreorder

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
    //lw sp, kernel_sp  这里弃用共同使用核心栈的方式 改为使用用户内核栈
    //注：内核栈的销毁时机为
    //偏移为 12
    lw k0, curproc
    lw sp, 12(k0)

    //注意！！系统调用不能和调度器共同使用核心栈! 否则会出现数据覆盖的错误!
    nop
    jal SystemCall
    nop

    lw k0, after_exec
    nop
    bnez k0, exec_return_add
    nop

    
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

    //返回前确保填入tlb 现在的sp是用户sp
    //

    STI
    
    nop
    jr v1
	nop

    //exec特殊处理
exec_return_add:
    sw zero, after_exec
    //恢复快照
    lw sp, curtf
    RESTORE_TF
    nop

    //更新entryhi(ASID)
    lw k1, curasid
    mtc0	k1,CP0_ENTRYHI

    //主动进入时钟
    mfc0 k1, CP0_STATUS
    or k1, k1, 0x2
    mtc0 k1, CP0_STATUS
    //la k0, sys_exec_back_add
    mtc0 v1, CP0_EPC
    //la k0, 
    nop
    j handle_int
    nop 


sys_exec_back_add:


    
    
    nop
    jr v1
	nop


    //子进程返回位置
.global child_proc_return_add
child_proc_return_add:
    //恢复快照
    lw sp, curtf
    RESTORE_TF
    nop

    //更新entryhi(ASID)
    lw k1, curasid
    mtc0	k1,CP0_ENTRYHI

    STI
    
    nop
    jr v1
	nop


END(handle_sys)


