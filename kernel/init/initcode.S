#include "../inc/syscall.h"
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

.globl uinit
//exec("init.o", 0);
init_path_: .asciiz "init_path_\n"

uinit:
    nop
    nop
    li k0, 0x00000000
    j k0

    nop
    nop
    nop

    move	a1, zero
    li	a0, 0xffffffff
    li	v0, SYS_exec
    //li	$v0, SYS_sysTest

    syscall
    nop
    jr	ra
    nop
    nop


.globl enduinit
enduinit:
    nop
