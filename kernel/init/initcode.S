#include "../inc/syscall.h"

.globl uinit
//exec("init.o", 0);
init_path_: .asciiz "init_path_\n"
uinit:
    move	$a1, $zero
    //la	$a0, init_path_
    li	$a0, 0xffffffff
    li	$v0, SYS_exec
    //li	$v0, SYS_sysTest

    syscall
    nop
    jr	$ra
    nop
    nop


.globl enduinit
enduinit:
    nop
