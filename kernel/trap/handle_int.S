.set nomips16
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include "../inc/trap.h"

LEAF(handle_int)

SAVE_TF
CLI
.set	at
mfc0	t0, CP0_CAUSE
mfc0	t2, CP0_STATUS
and	t0, t2

andi	t1, t0, STATUSF_IP4
bnez	t1, scheduler
nop

eret
nop
END(handle_int)