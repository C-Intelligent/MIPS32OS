#pragma once
#include "../types.h"

u_int _mips_atomic_xchg(volatile u_int *addr, u_int newval);


//CP0_STATUS $12  STATUS_CU0 0x10000000  (STATUS_CU0 | 0x1)
static inline void
cli(void)
{
  asm volatile("mfc0	$k0, $12;"\
  				"li	$k1, 0x10000001;"\
				"or	$k0, $k1;xor	$k0, 0x1;"\
				"mtc0	$k0, $12;");
}

static inline void
sti(void)
{
  asm volatile("mfc0	$k0,	$12;"\
  				"li	$k1, 0x10000001;"\
				"or	$k0, $k1;"\
				"mtc0	$k0, $12;");
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
};