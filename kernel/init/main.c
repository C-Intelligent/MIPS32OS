#include "../inc/mfp_io.h"
#include "../inc/printf.h"
#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/memlayout.h"

void alloc_test();

int 
main(void) {
  printf("\nHello Operating System!\n");
  
  /*初始化空闲页链表*/
  /*页表中存线性地址，核心态的虚拟地址与线性地址相同*/
  /*只有线性地址和虚拟地址能访问*/
  phys_page_allocator_init((void*)MAPPED_RAM_START, (void*)MAPPED_RAM_STOP);  // phys page allocator
  alloc_test();

  /*为核心创建页表并初始化*/
  init_kpg_table();


  /*文件系统初始化*/

  // process_init();
  /*初始化用户进程  os通过回复现场返回进程，因此进程初始化需要自建第一个现场allocproc*/
  // userinit();

  // mpmain();

  printf("finish init\n");


  while (1);

  return 0;
}

void alloc_test() {
  /*线性地址*/
  char* mpk = kalloc();
  printf("%x", (u_int)mpk);
  mpk[0] = 'a';
  mpk[1] = 'a';
  mpk[2] = 'a';
  mpk[3] = '\0';
  printf("%s", mpk);
}

// Common CPU setup code.
static void
mpmain(void)
{
  // cprintf("cpu%d: starting\n", cpu->id);
  // idtinit();       // load idt register
  // xchg(&cpu->started, 1); // tell startothers() we're up
  scheduler();     // start running processes
}