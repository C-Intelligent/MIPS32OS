#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/timer.h"

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

extern u_int rounds;

int
sys_sleep(int t) {
  //通过轮数计时(并不准确)
  u_int n = (t / ROUNDTICKS);
  u_int round0 = rounds;
  lock_ptable();
  while(rounds - round0 < n){
    if(curproc->killed){
      return -1;
    }
    sleep_on_train(&rounds);
  }
}

extern int disprocs(void);
int
sys_disprocs(void) {
  disprocs();
  return 0;
}
