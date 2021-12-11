#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>


LEAF(_mips_atomic_xchg)
    addiu sp, sp, -12
    sw ra, (sp)
    sw t0, 4(sp)

    //由于是单核处理器 且在关中断状态下 操作肯定是原子的
    ll t0, (a0)
    sc a1, (a0)
    move v0, t0

    lw t0, 4(sp)
    lw ra, (sp)
    addiu sp, sp, 12
    jr ra
    nop
END(_mips_atomic_xchg)