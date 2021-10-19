#pragma once
#include "../types.h"

static inline u_int
_mips_atomic_xchg(volatile u_int *addr, u_int newval)
{
//   u_int result;
  
  // The + in "+m" denotes a read-modify-write operand.
  asm volatile("ll $t0, 0($a0); sc $t0, 0($a1); addiu $v0, $t0, 0;" \
                : : :);
//   return result;
}

static inline void
cli(void)
{
  asm volatile("cli");
}

static inline void
sti(void)
{
  asm volatile("sti");
}

