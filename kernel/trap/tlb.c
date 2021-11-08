#include "../inc/printf.h"
#include "../inc/defs.h"

// EntryHi 包含 VPN2 和 ASID 字段
// VPN2 就是 TLB 表项对应的虚拟页表号，在 MIPS 设计中，一个 TLB 表项存储的是两个相邻虚拟页对应的物理页框号，所以 VPN2 只有 21 位（除去页内偏移12位和第13位）
// ASID 字段，是当前进程的ASID，在对 TLB 访问查询中起到验证的作用（具体见 ASID 部分）。
// EntryLo0-1
// PFN 对应的物理页框号
// D(dirty) 标志位，置位时允许写入；0 不允许写入，若写入则发生异常
// V(valid) 有效位
// G(global) 表明该 TLB 表项是全局的还是属于特定进程的，若是全局的，则对该 TLB 表项的读取都会无视 ASID 是否匹配

//注：每次重填8KB，否则会出问题
//注：已重写为汇编形式
void __handle_tlb() {
    u_int bada, entryHi, context_, enlo0, enlo1, epc;
    asm volatile("mfc0 %0, $8; "\
                "mfc0 %1, $10; "\
                "mfc0 %2, $4;" \
                "mfc0 %3, $2;" \
                "mfc0 %4, $3;" \
                "mfc0 %5, $14;" \
                : "=r" (bada), "=r" (entryHi)
                , "=r" (context_), "=r" (enlo0), "=r" (enlo1), "=r" (epc)); //#获取发生tlb的指令地址
    // asm volatile("mfc0 %0, $10" : "=r" (entryHi)); //#获取发生tlb的指令地址

    printf("%x\n", epc);
    
    entryHi = bada & 0xffffe000;

    u_int pa[2];
    allocate8KB(entryHi, pa);
    enlo0 = pa[0];
    enlo0 = enlo0 >> 6;
    enlo0 |= 0x00000017;
    enlo1 = pa[1];
    enlo1 = enlo1 >> 6;
    enlo1 |= 0x00000017;
    
    // printf("refill: %x----->%x\n", bada, pa);

    // #define CP0_ENTRYLO0 $2
    // #define CP0_ENTRYLO1 $3
    // 31-12 PPN  C:010(第345位 uncached) 2:准写位 1:有效位 0:全局位
    
    /*
    if ((bada >> 12) & 1) {
        //奇数 enlo1
        enlo1 = pa;
        enlo1 |= 0x00000017;
    } else {
        enlo0 = pa;
        enlo0 |= 0x00000017;
    }
    */
    
    u_int pg_mask = 0x00000180;

    asm volatile("mtc0 %0, $2; "\
                "mtc0 %1, $3; "\
                "mtc0 %2, $10; "\
                "mtc0 %3, $5;"\
                "nop; tlbwr; eret; nop;" \
                :: "r" (enlo0), "r" (enlo1), "r" (entryHi), "r" (pg_mask)); //重填 随机写入tlb
    // printf("htlb: %x%x%x%x\n", bada, entryHi, context_, pa);
}