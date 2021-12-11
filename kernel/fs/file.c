#include "../inc/file.h"
#include "../inc/printf.h"
#include "../inc/types.h"
#include "../inc/defs.h"
#include "../drivers/ff.h"
#include "../drivers/console.h"

//文件打开表
struct {
  struct spinlock lock;
  struct file file[NFILE];
} ftable;

struct file _stdin_f_;
struct file _stdout_f_;

struct file *
get_std_out_f() {
  _stdout_f_.ref++;
  return &_stdout_f_;
}

struct file *
get_std_in_f() {
  _stdin_f_.ref++;
  return &_stdin_f_;
}

// Allocate a file structure.
struct file*
filealloc(void)
{
  struct file *f;

  acquire(&ftable.lock);
  for(f = ftable.file; f < ftable.file + NFILE; f++){
    if(f->ref == 0){
      f->ref = 1;
      release(&ftable.lock);
      return f;
    }
  }
  release(&ftable.lock);
  return 0;
}

int
filewrite(struct file *f, char *addr, int n)
{
  // printf("write file: %x, writable: %x\n", f, f->writable);
  int r;

  if(f->writable == 0)
    return -1;
  
  if(f->type == FD_PIPE)
    return pipewrite(f->pipe, addr, n);
  
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

int
fileread(struct file *f, char *addr, int n)
{
  int r;

  if(f->readable == 0)
    return -1;

  if(f->type == FD_PIPE)
    return piperead(f->pipe, addr, n);

  if (f->type == FD_SERIAL) {
    int i;
    char c;
    for(i=0; i < n; i++){
      c = (char)getchar();
      
      addr[i] = c;
    }
    return i;
  }

  if(f->type == FD_FAT){
    //暂未考虑读写锁安全问题  后续要加上  lock, unlock
    FRESULT res;
    u_int br = 0;
    f_read (f->fp, addr, n, &br);
    if (br > 0) f->off += br;
    return br == n ? n : -1;
  }
  panic("fileread");
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

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("fileclose");
  if(--f->ref > 0){
    release(&ftable.lock);
    return;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);
  
  if(ff.type == FD_PIPE) {
    pipeclose(ff.pipe, ff.writable);
  }
    //pipeclose(ff.pipe, ff.writable);
  else if(ff.type == FD_FAT){
    f_close(ff.fp);
  }
}

FATFS fs;
char curcwd[128];

void fs_init() {
    /*初始化磁盘FAT驱动*/
    const TCHAR *path = "0:/";
    uint8_t opt = 0;
    FRESULT res = f_mount(&fs, path, opt);
    FIL fp;

    f_getcwd(curcwd, 128);

    /*初始化标准输入输出流*/
    _stdout_f_.type = FD_SERIAL;
    _stdout_f_.readable = 0;
    _stdout_f_.writable = 1;
    _stdout_f_.ref = 1;
    _stdout_f_.fp = 0;
    _stdout_f_.off = 0;

    _stdin_f_.type = FD_SERIAL;
    _stdin_f_.readable = 1;
    _stdin_f_.writable = 0;
    _stdin_f_.ref = 1;
    _stdin_f_.fp = 0;
    _stdin_f_.off = 0;
    
    printf("end fs_init!\n");
}

int chdir(const char *path) {
  // printf("chdir :%s\n", path);
  FRESULT res = f_chdir(path);
  if (FR_OK == res) {
    res = f_getcwd(curcwd, 128);
    return res;
  }
  else return res;
}


void pwd() {
  printf("%s\n", curcwd + 2);
}

void ls() {
  FRESULT res;
  FILINFO fno;
  DIR dp;
  f_opendir(&dp, curcwd);
  while (1)
  {
    res = f_readdir(&dp, &fno);
    if(res != FR_OK || fno.fname[0]==0 )
    {
        break;		// 读取失败或者读取完所有条目
    }
    if (fno.fname[0] == '.') {
      if (fno.fname[1] == 0) continue;
      if (fno.fname[1] == '.' && fno.fname[2] == 0) continue;
    }
    printf("%s ", fno.fname);
  }
  f_closedir(&dp);
  printf("\n");
}


/*
文件目录项0CH字节的值：
1. 此值为18H时，文件名和扩展名都小写。
2. 此值为10H时，文件名大写而扩展名小写。
3. 此值为08H时，文件名小写而扩展名大写。
4. 此值为00H时，文件名和扩展名都大写*/

  // //切换小写
  // if (IsUpper(c) && (dir[DIR_NTres] & (i >= 9 ? NS_EXT : NS_BODY)))
  //   c += 0x20;          /* To lower */