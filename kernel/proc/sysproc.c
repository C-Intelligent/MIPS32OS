#include "../inc/types.h"
#include "../inc/defs.h"

extern struct proc *curproc;

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;
}

int
sys_wait(void)
{
  return wait();
}

//向上增长堆区
u_int
sys_sbrk(int n)
{
  u_int addr = curproc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}