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
    SAVE_TF

    lw k0, tmp_sp
    sw	k0,TF_REG29(sp)
    
    CLI

    move a0, sp

    //恢复sp
    lw sp, tmp_sp

    jal SystemCall
    nop

    STI
    //恢复快照
    lw sp, curtf
    RESTORE_TF
    //lw sp, tmp_sp  如果是exec 则无需恢复sp 因此需要在将curtf中的sp存储为tmp_sp
    jr	ra
    nop

END(handle_sys)
