#include "../inc/types.h"
#include "../inc/syscall.h"
#include "../inc/printf.h"
#include "../inc/mips/mips32.h"
#include "../inc/defs.h"

extern struct proc *proc;

int sys_sysTest(int a, int b, int c, int d) {
  printf("system call!\n");
  return 0;
}

int sys_exec(char* path, int argc, char** argv) {
  //由于第一个进程启动时存在的空间变换问题可能导致出错 这里约定全1 path为初始化地址
  // printf("sys_exec! pathadd: 0x%x\n", path);
  if (0xffffffff == path) {
    char *arr = "exec";
    return exec("bin/console", 1, &arr);
  }
  else return exec(path, argc, argv);
  return 0;
}
/*
#define v0      $2      
#define v1      $3
#define a0      $4      
#define a1      $5
#define a2      $6
#define a3      $7
*/

extern int sys_write(int fd, char* buf, int n);
extern int sys_read(int fd, char* buf, int n);
extern int sys_fork(void);
extern int sys_exit(void);
extern int sys_wait(void);
extern int sys_uname(char* name, int len);
extern int sys_pwd(char* cwd, int len);
extern int sys_chdir(const char *path);
extern int sys_close(int);
extern int sys_pipe(int*);
extern int sys_open(char* path, int omode);
extern int sys_fstat(int fd, struct stat *st);
extern int sys_dup(int newfd);
extern int sys_sleep(int t);
extern int sys_unlink(char*);
extern int sys_link(char*, char*);
extern int sys_mkdir(char*);
extern int sys_disprocs(void);
extern u_int sys_sbrk(int n);

void
SystemCall(struct trapframe *tf)
{
  //sti();
  //printf("syscall epc:%x \n", tf->cp0_epc);
  switch(tf->regs[2]){
    case SYS_sysTest: 
      tf->regs[2] = sys_sysTest(tf->regs[4], tf->regs[5], tf->regs[6], tf->regs[7]);
      tf->cp0_epc += 4;
      break;
    case SYS_uname: 
      tf->regs[2] = sys_uname(tf->regs[4], tf->regs[5]);
      tf->cp0_epc += 4;
      break;
    case SYS_pwd: 
      tf->regs[2] = sys_pwd(tf->regs[4], tf->regs[5]);
      tf->cp0_epc += 4;
      break;
    case SYS_chdir: 
      tf->regs[2] = sys_chdir(tf->regs[4]);
      tf->cp0_epc += 4;
      break;
    case SYS_exec: 
      tf->regs[2] = sys_exec(tf->regs[4], tf->regs[5], tf->regs[6]);
      extern u_int after_exec;
      if (!after_exec) tf->cp0_epc += 4;
      break;
    case SYS_write:
      // printf("write source addr: %x\n", tf->regs[5]);
      tf->regs[2] = sys_write(tf->regs[4], tf->regs[5], tf->regs[6]);
      tf->cp0_epc += 4;
      break;
    case SYS_read:
      tf->regs[2] = sys_read(tf->regs[4], tf->regs[5], tf->regs[6]);
      tf->cp0_epc += 4;
      break;
    case SYS_fstat:
      tf->regs[2] = sys_fstat(tf->regs[4], tf->regs[5]);
      tf->cp0_epc += 4;
      break;
    case SYS_open:
      tf->regs[2] = sys_open(tf->regs[4], tf->regs[5]);
      tf->cp0_epc += 4;
      break;
    case SYS_link:
      tf->regs[2] = sys_link(tf->regs[4], tf->regs[5]);
      tf->cp0_epc += 4;
      break;
    case SYS_unlink:
      tf->regs[2] = sys_unlink(tf->regs[4]);
      tf->cp0_epc += 4;
      break;
    case SYS_mkdir:
      tf->regs[2] = sys_mkdir(tf->regs[4]);
      tf->cp0_epc += 4;
      break;
    case SYS_pipe:
      tf->regs[2] = sys_pipe(tf->regs[4]);
      tf->cp0_epc += 4;
      break;
    case SYS_dup:
      tf->regs[2] = sys_dup(tf->regs[4]);
      tf->cp0_epc += 4;
      break;
    case SYS_close:
      tf->regs[2] = sys_close(tf->regs[4]);
      tf->cp0_epc += 4;
      break;
    case SYS_sbrk:
      tf->regs[2] = sys_sbrk(tf->regs[4]);
      tf->cp0_epc += 4;
      break;
    case SYS_sleep:
      tf->regs[2] = sys_sleep(tf->regs[4]);
      tf->cp0_epc += 4;
      break;
    case SYS_disprocs:
      tf->regs[2] = sys_disprocs();
      tf->cp0_epc += 4;
    case SYS_fork: 
      tf->regs[2] = sys_fork();
      tf->cp0_epc += 4;
      break;
    case SYS_wait:
      tf->regs[2] = sys_wait();
      tf->cp0_epc += 4;
      break;
    case SYS_exit:
      sys_exit();
      break;
    default: 
      //panic("syscall err");
      printf("syscall err: wrong syscall number\n");
      tf->cp0_epc += 4;
      break;
  }
}


