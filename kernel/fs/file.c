#include "../inc/file.h"
#include "../inc/printf.h"
#include "../inc/types.h"
#include "../inc/defs.h"
#include "../drivers/ff.h"

//文件打开表
struct {
  struct spinlock lock;
  struct file file[NFILE];
} ftable;

struct file _stdout_f_;

struct file *
get_std_out_f() {
  _stdout_f_.ref++;
  return &_stdout_f_;
}

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


// Increment ref count for file f.
struct file*
filedup(struct file *f)
{
  
  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  return f;
}

FATFS fs;

void fs_init() {
    /*初始化磁盘FAT驱动*/
    const TCHAR *path = "/";
    uint8_t opt = 0;
    FRESULT res = f_mount(&fs, path, opt);
    FIL fp;
    res = f_open (&fp, "/test.txt", FA_READ);
    f_close(&fp);
    // char buffer[4096];
    // u_int br = 0;
    // FRESULT res2 = f_read (&fp, buffer, 4096, &br); 
    // printf("%u\n", br);

    // f_mkdir ("test");
    /*-----------------*/

    /*初始化标准输入输出流*/
    _stdout_f_.type = FD_SERIAL;
    _stdout_f_.readable = 1;
    _stdout_f_.writable = 1;
    _stdout_f_.ref = 1;
    _stdout_f_.fp = 0;
    _stdout_f_.off = 0;
    
    printf("end fs_init!\n");
}