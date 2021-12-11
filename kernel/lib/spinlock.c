#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/mips/mips32.h"
#include "../inc/printf.h"

void
initlock(struct spinlock *lk, char *name)
{
  lk->name = name;
  lk->locked = 0;
}

// Acquire the lock.
void
acquire(struct spinlock *lk)
{
  //由于内核态不开中断 故这里不开中断
  // The xchg is atomic.
  while(_mips_atomic_xchg(&lk->locked, 1) != 0);
  
}

// Release the lock.
void
release(struct spinlock *lk)
{
  // lk->pcs[0] = 0;

  _mips_atomic_xchg(&lk->locked, 0);

}

void
pushcli(void)
{
}

void
popcli(void)
{
}