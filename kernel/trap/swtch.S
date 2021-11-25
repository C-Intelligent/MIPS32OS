#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include "../inc/trap.h"

.set noreorder
//void  swtch(struct trapframe* save_to, struct trapframe * to_load);
LEAF(swtchk2u)
    .set at

    sw		sp, kernel_sp

    //临时保存a1  
    sw a1, tmp_sp

    //a0 -> sp
    move sp, a0
    addiu	sp,sp, TF_SIZE //SAVE_TF中还要再减
    //保存了返回地址ra
    SAVE_TF
    lw	k0, kernel_sp
    sw	k0,TF_REG29(sp)

    //清除时间片并且开中断
    lw sp, kernel_sp
    

    //TIMER_ADDR          0xB0500000
    //<get_TCSR0>
    //TIMER_TCSR0_T0INT   (1 << 8)
    //TIMER_TCSR0_LOAD0   (1 << 5)    // Load Timer 0
    //TIMER_TCSR0_ENIT0   (1 << 6)
    //TIMER_TCSR0_ARHT0   (1 << 4)
    //TIMER_TCSR0_UDT0    (1 << 1)    // Up/Down Count Timer 0
    //TIMER_TCSR0_ENT0    (1 << 7)    // Enable Timer 0
    //0x1d2  8 7 6 4 1  0xd2
    //0x52 6 4 1
    

//    lui	k1,0xb050
//    lw	k0,0(k1)
//    ori	k0,k0,0x100
//    sw	k0,0(k1)
//    lw	k0,0(k1)
//    lw	k0,0(k1)
//    lw	k0,0(k1)
//test_timer:
//    lw	k0,0(k1)
//    lw	k0,0(k1)
//    j test_timer

  /*  
    //重置时钟
    lui	k1,0xb050
    //disable
    lw	k0,0(k1)
    ori k0, 0x80
    sw	k0,0(k1)
    //init
    sw zero,0(k1)
    lui k0, 0x52
    sw	k0,0(k1)
    lui k0, 0xd2
    sw	k0,0(k1)
*/
    nop
    jal disable_timer0
    nop

    nop
    jal timer_init
    nop

    nop
    jal clear_timer0_int
    nop

    STI

    //a1 -> sp
    lw sp, tmp_sp
    RESTORE_TF
    mfc0	k0,CP0_EPC

    j k0
	nop

END(swtchk2u)