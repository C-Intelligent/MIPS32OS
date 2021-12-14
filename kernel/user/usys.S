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

LEAF(fork)
    li	v0, SYS_fork
    syscall
    jr	ra
    nop
END(fork)

LEAF(exit)
    li	v0, SYS_exit
    syscall
    jr	ra
    nop
END(exit)

LEAF(wait)
    li	v0, SYS_wait
    syscall
    jr	ra
    nop
END(wait)

LEAF(read)
    li	v0, SYS_read
    syscall
    jr	ra
    nop
END(read)

LEAF(uname)
    li	v0, SYS_uname
    syscall
    jr	ra
    nop
END(uname)

LEAF(pwd)
    li	v0, SYS_pwd
    syscall
    jr	ra
    nop
END(pwd)

LEAF(chdir)
    li	v0, SYS_chdir
    syscall
    jr	ra
    nop
END(chdir)

LEAF(pipe)
    li	v0, SYS_pipe
    syscall
    jr	ra
    nop
END(pipe)

LEAF(close)
    li	v0, SYS_close
    syscall
    jr	ra
    nop
END(close)

LEAF(sbrk)
    li	v0, SYS_sbrk
    syscall
    jr	ra
    nop
END(sbrk)

LEAF(open)
    li	v0, SYS_open
    syscall
    jr	ra
    nop
END(open)

LEAF(fstat)
    li	v0, SYS_fstat
    syscall
    jr	ra
    nop
END(fstat)

LEAF(dup)
    li	v0, SYS_dup
    syscall
    jr	ra
    nop
END(dup)

LEAF(sleep)
    li	v0, SYS_sleep
    syscall
    jr	ra
    nop
END(sleep)

LEAF(mkdir)
    li	v0, SYS_mkdir
    syscall
    jr	ra
    nop
END(mkdir)

LEAF(unlink)
    li	v0, SYS_unlink
    syscall
    jr	ra
    nop
END(unlink)

LEAF(link)
    li	v0, SYS_link
    syscall
    jr	ra
    nop
END(link)

LEAF(disprocs)
    li	v0, SYS_disprocs
    syscall
    jr	ra
    nop
END(disprocs)


LEAF(pause)
here_pause:
    nop
    nop
    j here_pause
    nop
    nop
END(pause)


/*
LEAF(safebytecpy)
    addiu sp, sp, -24
    sw ra, (sp)
    sw t1, 4(sp)

    //a0 addr  a1  byte
    li t1, 0x0000
    lw t1, ()
    lw t1, 4(sp)
    lw ra, (sp)
    addiu sp, sp, 24
    jr	ra
    nop
END(safebytecpy)
*/