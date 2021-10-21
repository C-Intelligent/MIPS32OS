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

struct trapframe {
  // registers as pushed by pusha
  u_int edi;
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