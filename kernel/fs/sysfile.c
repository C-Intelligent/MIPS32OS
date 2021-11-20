#include "../inc/file.h"
#include "../inc/defs.h"
//#include "../inc/printf.h"

extern struct proc *curproc;

int
sys_write(int fd, char* buf, int n)
{
  struct file* f;
  //printf("curproc->ofile[fd]:%x\n", curproc->ofile[fd]);
  if(fd < 0 || fd >= NOFILE || (f=curproc->ofile[fd]) == 0) return -1;
  return filewrite(f, buf, n);
}