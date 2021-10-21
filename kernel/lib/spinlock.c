// Mutual exclusion spin locks.
#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/mips/mips32.h"
// #include "param.h"
// #include "x86.h"
// #include "memlayout.h"
// #include "mmu.h"
// #include "proc.h"

void
initlock(struct spinlock *lk, char *name)
{
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
void
acquire(struct spinlock *lk)
{
  //需要关中断吗 为什么?
  pushcli(); // disable interrupts to avoid deadlock.
  // if(holding(lk))
  //   panic("acquire");

  // The xchg is atomic.
  // It also serializes, so that reads after acquire are not
  // reordered before it. 

  
  while(_mips_atomic_xchg(&lk->locked, 1) != 0);
  
  // Record info about lock acquisition for debugging.
  // lk->cpu = cpu;
  // getcallerpcs(&lk, lk->pcs);
}

// Release the lock.
void
release(struct spinlock *lk)
{
  // if(!holding(lk))
  //   panic("release");

  // lk->pcs[0] = 0;
  // lk->cpu = 0;

  _mips_atomic_xchg(&lk->locked, 0);

  popcli();
}

// Check whether this cpu is holding the lock.
// int
// holding(struct spinlock *lock)
// {
//   return lock->locked && lock->cpu == cpu;
// }


// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.

void
pushcli(void)
{
  // int eflags;
  
  // eflags = readeflags();
  // cli();
  // if(cpu->ncli++ == 0)
  //   cpu->intena = eflags & FL_IF;
}

void
popcli(void)
{
  // if(readeflags()&FL_IF)
  //   panic("popcli - interruptible");
  // if(--cpu->ncli < 0)
  //   panic("popcli");
  // if(cpu->ncli == 0 && cpu->intena)
  //   sti();
}