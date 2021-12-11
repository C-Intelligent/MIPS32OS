.set nomips16
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
//#include "../inc/defs.h"
#include "tlb.h"

//tlb中需要保存所有通用寄存器！
//和普通函数不同

.set noreorder
.text
LEAF(handle_tlb)
	.set at

//注：每次重填8KB，否则会出问题 entryHi需要设置asid
//所有空间全局可访问（以后可优化）
//注意不能设置全局位
//!!!要切回kernal_sp(start.S中)

	sw sp, tmp_sp

	//恢复sp
	//-----------特殊情形检测----------------//
    //--epc属于内核空间
    mfc0 k0, CP0_EPC
    li k1, 0x80000000
    and k0, k0, k1
	nop
	nop
    bnez k0, tlb_j_from_kernel
	nop
	nop
    //--------------------------------------//
    lw sp, kernel_sp

tlb_j_from_kernel:
    sw sp, kernel_sp

    TLB_SAVE_TF

	addiu sp, sp, -8

	//这一句debug用
	mfc0 s0, CP0_EPC
 	mfc0	s0,CP0_BADVADDR

    //u_int pa[2];
    //allocate8KB(entryHi, pa);
    //传入pa
 	addiu	a1,sp,0
    //传入entryHi
 	move	a0,s0
	mfc0 k0, CP0_EPC
	move a2, k0
	nop
 	jal	 allocate8KB
    nop

	//entryHi = bada & 0xffffe000;
 	ins	s0,zero,0x0,0xd

	//entryHi |= asid
	lw t1, curasid
 	or	s0,s0, t1

    //enlo0 = pa[0];
 	lw	v0,0(sp)
    //enlo0 = enlo0 << 6;
 	srl	v0,v0,0x6
    //enlo1 = pa[1];
 	lw	v1,4(sp)
    //enlo1 = enlo1 << 6;
 	srl	v1,v1,0x6
    //enlo0 |= 0x00000017; enlo1 |= 0x00000017;  => 0x00000017
	//010(C)  1(D)  1(V)  0(G)
	//111 1 1 0 0x3e
	//最后一位全局位不能设置
 	ori	v0,v0,0x3e
 	ori	v1,v1,0x3e

    //u_int pg_mask = 0x00000180;
 	li	a0,0x180
	nop
	mtc0	a0,CP0_PAGEMASK
	nop
	mtc0	s0,CP0_ENTRYHI
 	
	nop
	nop
 	mtc0	v1,CP0_ENTRYLO1
	nop
	nop
 	mtc0	v0,CP0_ENTRYLO0
	nop
	nop
 	
 	nop
	nop
	ehb
 	tlbwr
	
	nop
	

	addiu sp, sp, 8
	TLB_RESTORE_TF

	//开中断  如果从调度器过来  就不开 且不恢复用户sp
	mfc0 k0, CP0_EPC

	lw k0, from_sche

	lw		sp, tmp_sp 

	nop
	nop
	bnez k0, j_from_sche
	nop
	nop

	//恢复用户sp
	
	//lw		sp, tmp_sp 

	//开中断的瞬间可能会跳到int处理程序中
	//为解决这个问题 这里主动检测时钟片是否到期
	//若到期 则主动进入切换
	/*
	lui	k1,0xb050
    lw	k0,0(k1)
	andi k0, k0, 0x100
	bnez k0, j_to_handle_int
	nop
	*/
	

    STI  
	
	nop
    eret
 	nop

j_from_sche:
	sw zero, from_sche

	nop
	nop
	
    eret
 	nop
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