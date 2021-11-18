#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include "../inc/trap.h"

//void  swtch(struct trapframe* save_to, struct trapframe * to_load);
LEAF(swtch)
    .set at

    subu	sp,sp, 8
    sw a0, 0(sp)
    sw a1, 4(sp)
    //临时保存sp
    sw sp, tmp_sp

    //a0 -> sp
    move sp, a0
    SAVE_TF

    //a1 -> sp
    lw sp, tmp_sp
    lw sp, 4(sp)
    RESTORE_TF
    
    lw sp, tmp_sp
    //恢复现场运行
    addiu	sp,sp, 8
    jal ra
    nop
END(swtch)