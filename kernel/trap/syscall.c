#include "../inc/types.h"
#include "../inc/syscall.h"
#include "../inc/printf.h"
#include "../inc/mips/mips32.h"
#include "../inc/defs.h"

extern struct proc *proc;

// int sys_sysTest(int a, int b, int c, int d) {
//   printf("system call! %d - %d - %d - %d \n", a, b, c, d);
//   return a + b + c + d;
// }

int sys_sysTest(int a, int b, int c, int d) {
  printf("system call!\n");
  return 0;
}

int sys_exec(char* path, int argc, char** argv) {
  //由于第一个进程启动时存在的空间变换问题可能导致出错 这里约定全1 path为初始化地址
  printf("sys_exec! pathadd: 0x%x\n", path);
  if (0xffffffff == path) {
    printf("to exec console\n");
    return exec("console", 0, 0);
  }
  // else return exec(path, argc, argv);
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
extern void tlb_out(u_int va);

void
SystemCall(struct trapframe *tf)
{
  if (tf->regs[2] == SYS_sysTest) 
    tf->regs[2] = sys_sysTest(tf->regs[4], tf->regs[5], tf->regs[6], tf->regs[7]);
  else if (tf->regs[2] == SYS_exec) {
    tf->regs[2] = sys_exec(tf->regs[4], tf->regs[5], tf->regs[6]);
    tlb_out(tf->regs[31]);
  }
  else if (tf->regs[2] == SYS_write) 
    tf->regs[2] = sys_write(tf->regs[4], tf->regs[5], tf->regs[6]);
}


