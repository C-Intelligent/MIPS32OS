#include "../inc/file.h"
#include "../inc/defs.h"
#include "../inc/string.h"
#include "../inc/printf.h"

extern struct proc *curproc;

const char whitespace[] = " \t\r\n\v";
extern char curcwd[];

int
sys_write(int fd, char* buf, int n)
{
  struct file* f;
  //printf("curproc->ofile[fd]:%x\n", curproc->ofile[fd]);
  if(fd < 0 || fd >= NOFILE || (f=curproc->ofile[fd]) == 0) return -1;
  return filewrite(f, buf, n);
}

int
sys_read(int fd, char* buf, int n)
{
  struct file *f;
  if(fd < 0 || fd >= NOFILE || (f=curproc->ofile[fd]) == 0) return -1;
  return fileread(f, buf, n);
}

int 
sys_uname(char* name, int len) 
{
  safestrcpy(name, "SEU@2021", len);
}

int 
sys_pwd(char* cwd, int len) 
{
  
  char *d = curcwd;
  if (*d == '0') d++;
  if (*d == ':') d++;
  safestrcpy(cwd, d, len);
  return 0;
}

int
sys_chdir(const char *path)
{
  return chdir(path);
}

static int
fdalloc(struct file *f)
{
  int fd;

  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd] == 0){
      curproc->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}


int
sys_pipe(int *fd)
{
  struct file *rf, *wf;
  int fd0, fd1;

  if(pipealloc(&rf, &wf) < 0)
    return -1;
  fd0 = -1;

  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      curproc->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    return -1;
  }
  fd[0] = fd0;
  fd[1] = fd1;
  return 0;
}

int
sys_close(int fd)
{
  struct file *f;
  
  if(fd < 0 || fd >= NOFILE || (f=curproc->ofile[fd]) == 0) return -1;
  curproc->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

//也可以打开目录
//这里不提供创建目录的功能
int
sys_open(char* path, int omode)
{
  int fd;
  struct file *f;
  FNODE* fn;
  FRESULT res;

  //拼接为完整路径
  char fullpath[128];
  int cwdlen = strlen(curcwd);
  int plen = strlen(path);
  safestrcpy(fullpath, curcwd, sizeof(fullpath));
  
  if (plen + cwdlen > 127) {
    printf("path length too long\n");
    return -1;
  } 
  //检查相对性
  //忽略空白符
  char *s = path, *es = path + plen, *fpes = fullpath + cwdlen;
  //(非根目录)给最后一位补上/
  if (fullpath[cwdlen - 1] != '/') fullpath[cwdlen] = '/';
  while(s < es && strchr(whitespace, *s))
    s++;
  while (*s == '.' && *(s + 1) == '.' && *(s + 2) == '/') {
    s += 3;
    fpes--;
    while (fpes > fullpath && *fpes != '/') fpes--;
  }
  if (*s == '.') s++;
  //拼接
  if (*s) safestrcpy(fpes + 1, s, sizeof(fullpath) - (int)(fpes - fullpath));
  //非根目录去掉最后的斜杠
  int newflen = strlen(fullpath);
  if (fullpath[newflen - 1] == '/' && fullpath[newflen - 2] != ':')
    fullpath[newflen - 1] = '\0';

  // printf("fullpath:%s\n", fullpath);


  //查看是否已经被其他进程打开
  fn = namef(fullpath);
  if(!fn) {
    if ((f = filealloc()) == 0 || (f->fp = fnodealloc()) == 0) return -1;
    //先当作文件考虑
    res = f_open((FIL*)f->fp, fullpath, omode);
    // printf("omode: %x\n", omode);
    
    /*
    char fbuf[32], ch = 1;
    uint32_t br;
    f_read((FIL*)f->fp, &ch, 1, &br);
    while(ch) {
      printf("%c", ch);
      f_read((FIL*)f->fp, &ch, 1, &br);
    }
    printf("|||end\n");
    */
    
    if (FR_OK != res) {
      //再当作目录考虑
      res = f_opendir((DIR*)f->fp, fullpath);
      if (FR_OK != res) {
        fileclose(f);
        return -1;
      }
      f->fp->type = T_DIR;
      safestrcpy(((filinfo*)f->fp)->path, fullpath, 127);
    }
    else {
      safestrcpy(((filinfo*)f->fp)->path, fullpath, 127);
      f->fp->type = T_FIL;
    }
  }
  else {
    //已经被打开
    if ((f = filealloc()) == 0) return -1;
    f->fp = fn;
  }

  if((fd = fdalloc(f)) < 0){
    if(f)
      fileclose(f);
    return -1;
  }

  f->type = FD_FAT;
  f->off = 0;
  f->readable = !(omode & WRITE);
  f->writable = (omode & WRITE) || (omode & READ);
  //目录不具有写权限
  if (T_DIR == f->fp->type) f->writable = 0;
  return fd;
}


int
sys_fstat(int fd, struct stat *st) {
  struct file *f;
  if(fd < 0 || fd >= NOFILE || (f=curproc->ofile[fd]) == 0) return -1;
  return filestat(f, st);
}

int
sys_dup(int newfd)
{
  struct file *f;
  int fd;  //通常为输出或输入
  
  if(newfd < 0 || newfd >= NOFILE || (f=curproc->ofile[newfd]) == 0) return -1;

  if((fd=fdalloc(f)) < 0)
    return -1;
  filedup(f);
  return fd;
}

int sys_unlink(char* path) {
  FRESULT res = f_unlink(path);
  if (FR_OK != res) return -1;
}

int sys_link(char* old, char* new) {
  return -1;
}

int sys_mkdir(char* path) {
  FRESULT res = f_mkdir(path);
  if (FR_OK != res) return -1;
}