#include "../inc/mfp_io.h"
#include "../inc/printf.h"
#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/memlayout.h"
#include "../drivers/console.h"

void alloc_test();
void spaces_teat();

// extern void uinit();
void readtest();

void locktest();

int 
main(void) {
  printf("\nHello Operating System!\n");

  //locktest();

  /*初始化空闲页链表*/
  /*页表中存线性地址，核心态的虚拟地址与线性地址相同*/
  /*只有线性地址和虚拟地址能访问*/
  phys_page_allocator_init((void*)MAPPED_RAM_START, (void*)MAPPED_RAM_STOP);  // phys page allocator

  /*为核心创建页表并初始化*/
  init_kpg_table();

  /*陷入初始化*/
  trap_init();

  // alloc_test();
  /*
  printf("input:");
  char ch = (char)getchar();
  printf("getchar: %c\n", ch);
  */
  


  /*文件系统初始化*/
  fs_init();

  // uinit();

  // process_init();
  /*初始化第一个用户进程  os通过回复现场返回进程，因此进程初始化需要自建第一个现场allocproc*/
  userinit();
  printf("finish user init\n");

  // readtest();

  //初始化时钟中断
  timer_init();
  disable_timer0();
  
  printf("finish time init\n");
  // while (1) {
  //   int count = 0;
  //   while (count++ < 1000000);
  //   printf("----\n");
  // }
  
  //###############################
  //奇怪的东西
  u_int *addr = 0x00002234;
  *addr = 123;

  //###############################


  // idtinit();       // load idt register
  // xchg(&cpu->started, 1); // tell startothers() we're up
  scheduler();     // start running processes.;;

  printf("finish init\n");

  while (1);
}

extern struct file _stdin_f_;
extern int fileread(struct file *f, char *addr, int n);
extern int sys_read(int fd, char* buf, int n);

char*
gets(char *buf, int max)
{
  int i, cc;
  char ch;

  for(i=0; i+1 < max; ){
    // ch = (char)getchar();
    fileread(&_stdin_f_, &ch, 1);
    //if (ch == '\r') cputchar('\n');
    //cputchar(ch);

    buf[i++] = ch;
    if(ch == '\r' || ch == '\n')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
getcmd(char *buf, int nbuf)
{
  printf("$ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

void readtest() {
  static char buf[100];

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
      printf("input cmd: %s\n", buf);
  }
}

void spaces_teat() {
  //测试切换切换不同虚拟地址空间
}


void alloc_test() {
  /*线性地址*/
  char* mpk = kalloc();
  printf("%x", (u_int)mpk);
  mpk[0] = 'a';
  mpk[1] = 'a';
  mpk[2] = 'a';
  mpk[3] = '\0';
  printf("%s\n\n", mpk);
  int i = 0;
  u_int addr = 0x00002234;
  *((int*)addr) = i;
  printf("\n%x-->%d\n", addr, *((int*)addr));

  int a, b, c;

  for (i = 0; i < 100; i++) {
    addr = addr + 0x1000;
    *((int*)addr) = i;
    printf("%x-->%d\n", addr, *((int*)addr));
  }
  
}


void locktest() {
  struct spinlock lk;
  initlock(&lk, "testlock");
  // lk.locked = 1;
  // spinlock lk;
  acquire(&lk);
  printf("get lock\n");
  printf("locked\n");
  release(&lk);
  printf("release\n");
  acquire(&lk);
  printf("get lock\n");
  acquire(&lk);
  printf("get lock\n");
}