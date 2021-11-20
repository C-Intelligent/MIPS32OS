.set nomips16
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
//#include "../inc/defs.h"

.text
LEAF(handle_tlb)

//注：每次重填8KB，否则会出问题 entryHi需要设置asid
//所有空间全局可访问（以后可优化）
//注意不能设置全局位
//!!!要切回kernal_sp(start.S中)
 	addiu	sp,sp,-48
    sw	v0,44(sp)
    sw	v1,40(sp)
    sw	a0,36(sp)
    sw	a1,32(sp)
 	sw	ra,28(sp)
 	sw	s0,24(sp)

	//这一句debug用
	mfc0 s0, CP0_EPC
 	mfc0	s0,CP0_BADVADDR
    
    //entryHi = bada & 0xffffe000;
 	ins	s0,zero,0x0,0xd

    //u_int pa[2];
    //allocate8KB(entryHi, pa);
    //传入pa
 	addiu	a1,sp,16
    //传入entryHi
 	move	a0,s0
 	jal	 allocate8KB
    nop

	//entryHi |= asid
	lw t1, curasid
 	or	s0,s0, t1

    //enlo0 = pa[0];
 	lw	v0,16(sp)
    //enlo0 = enlo0 >> 6;
 	srl	v0,v0,0x6
    //enlo1 = pa[1];
 	lw	v1,20(sp)
    //enlo1 = enlo1 >> 6;
 	srl	v1,v1,0x6
    //enlo0 |= 0x00000017; enlo1 |= 0x00000017;  => 0x00000017
	//最后一位全局位不能设置
 	ori	v0,v0,0x16
 	ori	v1,v1,0x16

    //u_int pg_mask = 0x00000180;
 	li	a0,384
 	mtc0	v0,CP0_ENTRYLO0
 	mtc0	v1,CP0_ENTRYLO1
 	mtc0	s0,CP0_ENTRYHI
 	mtc0	a0,CP0_PAGEMASK
 	nop
 	tlbwr

    lw	v0,44(sp)
    lw	v1,40(sp)
    lw	a0,36(sp)
    lw	a1,32(sp)
 	lw	ra,28(sp)
 	lw	s0,24(sp)
 	addiu	sp,sp,48

	//恢复用户sp
	lw		sp, user_sp 

    eret
 	nop

END(handle_tlb)

LEAF(tlb_out)
 	addiu	sp,sp,-48
    sw	v0,44(sp)
    sw	v1,40(sp)
    sw	a0,36(sp)
    sw	a1,32(sp)
 	sw	ra,28(sp)
 	sw	s0,24(sp)

	move s0, a0
    
    //entryHi = bada & 0xffffe000;
 	ins	s0,zero,0x0,0xd

    //u_int pa[2];
    //allocate8KB(entryHi, pa);
    //传入pa
 	addiu	a1,sp,16
    //传入entryHi
 	move	a0,s0
 	jal	 allocate8KB
    nop

	//entryHi |= asid
	lw t1, curasid
 	or	s0,s0, t1

    //enlo0 = pa[0];
 	lw	v0,16(sp)
    //enlo0 = enlo0 >> 6;
 	srl	v0,v0,0x6
    //enlo1 = pa[1];
 	lw	v1,20(sp)
    //enlo1 = enlo1 >> 6;
 	srl	v1,v1,0x6
    //enlo0 |= 0x00000017; enlo1 |= 0x00000017;  => 0x00000017
	//最后一位全局位不能设置
 	ori	v0,v0,0x16
 	ori	v1,v1,0x16

    //u_int pg_mask = 0x00000180;
 	li	a0,384
 	mtc0	v0,CP0_ENTRYLO0
 	mtc0	v1,CP0_ENTRYLO1
 	mtc0	s0,CP0_ENTRYHI
 	mtc0	a0,CP0_PAGEMASK
 	nop
 	tlbwr

    lw	v0,44(sp)
    lw	v1,40(sp)
    lw	a0,36(sp)
    lw	a1,32(sp)
 	lw	ra,28(sp)
 	lw	s0,24(sp)
 	addiu	sp,sp,48

    jr ra
 	nop

END(tlb_out)