#pragma once
#include "../types.h"

static inline u_int
_mips_atomic_xchg(volatile u_int *addr, u_int newval)
{
  asm volatile("ll $t0, 0($a0); sc $t0, 0($a1); addiu $v0, $t0, 0;" \
                : : :);
}

//CP0_STATUS $12  STATUS_CU0 0x10000000  (STATUS_CU0 | 0x1)
static inline void
cli(void)
{
  asm volatile("mfc0	$t0, $12;"\
  				"li	$t1, 0x10000001;"\
				"or	$t0, $t1;xor	$t0, 0x1;"\
				"mtc0	$t0, $12;");
}

static inline void
sti(void)
{
  asm volatile("mfc0	$t0,	$12;"\
  				"li	$t1, 0x10000001;"\
				"or	$t0, $t1;"\
				"mtc0	$t0, $12;");
}

struct trapframe {
  /* Saved main processor registers. */
	unsigned long regs[32];

	/* Saved special registers. */
	unsigned long cp0_status;
	unsigned long hi;
	unsigned long lo;
	unsigned long cp0_badvaddr;
	unsigned long cp0_cause;
	unsigned long cp0_epc;
	unsigned long pc;


  // registers as pushed by pusha
  // u_int edi;
//   uint esi;
//   uint ebp;
//   uint oesp;      // useless & ignored
//   uint ebx;
//   uint edx;
//   uint ecx;
//   uint eax;

//   // rest of trap frame
//   ushort gs;
//   ushort padding1;
//   ushort fs;
//   ushort padding2;
//   ushort es;
//   ushort padding3;
//   ushort ds;
//   ushort padding4;
//   uint trapno;

//   // below here defined by x86 hardware
//   uint err;
//   uint eip;
//   ushort cs;
//   ushort padding5;
//   uint eflags;

//   // below here only when crossing rings, such as from user to kernel
//   uint esp;
//   ushort ss;
//   ushort padding6;
};