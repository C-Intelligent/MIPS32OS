#include "../inc/file.h"
#include "../inc/printf.h"
#include "../inc/print.h"

struct file _stdout_f_;

int
filewrite(struct file *f, char *addr, int n)
{
  int r;

  if(f->writable == 0)
    return -1;
  
  /*
  if(f->type == FD_PIPE)
    return pipewrite(f->pipe, addr, n);
  */
  if (f->type == FD_SERIAL) {
    myoutput(0, addr, n);
    return n;
  }
  
  if(f->type == FD_FAT){
    
    //暂未考虑读写锁安全问题  后续要加上  lock, unlock
    FRESULT res;
    u_int bw = 0;
    f_write (f->fp, addr, n, &bw);
    if (bw > 0) f->off += bw;
    return bw == n ? n : -1;
  }
  panic("filewrite");
}
