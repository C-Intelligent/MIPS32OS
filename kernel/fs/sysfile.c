#include "../inc/file.h"
#include "../inc/defs.h"
#include "../inc/string.h"
#include "../inc/printf.h"

extern struct proc *curproc;

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
  extern char curcwd[];
  safestrcpy(cwd, curcwd + 2, len);
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