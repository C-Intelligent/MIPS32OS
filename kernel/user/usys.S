#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include "../inc/syscall.h"

LEAF(sysTest)

li	v0, SYS_sysTest

syscall

jr	ra
nop

END(sysTest)

LEAF(exec)

li	v0, SYS_exec

syscall

jr	ra
nop

END(exec)

LEAF(write)
    li	v0, SYS_write
    syscall
    jr	ra
    nop
END(write)



LEAF(usys)

sw	a0,0(sp)
sw	a1,4(sp)
sw	a2,8(sp)
sw	a3,12(sp)
move	v0, a0

syscall

jr	ra
nop

END(usys)