#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/mips/mips32.h"
#include "../inc/mmu.h"
#include "../inc/printf.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;
int nextpid = 1;

int nextasid = 1;

// extern void trapret(void);

// current process.
extern struct proc *curproc;    
extern struct trapframe *curtf;
extern void load_code(pde_t *pgdir, const char* path);
extern pde_t *curpgdir; 

//调度器现场
struct trapframe scheduler_tf;



void
process_init(void)
{
  initlock(&ptable.lock, "ptable");
}

// 在forkret函数中调用执行结束后执行trapret进行从内核栈返回到用户栈
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot 
    // be run from main().
    first = 0;
    // initlog();
  }
  
  // Return to "caller", actually trapret (see allocproc).
}

//找到一个空闲进程槽并分配，初始化进程资源
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;  //栈顶指针

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;
  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  // p->asid = (p - ptable.proc) / sizeof(struct proc *) + 1;
  //这个设计导致不能开太多进程（希望以后优化）
  p->asid = nextasid++;
  release(&ptable.lock);

  //分配内核栈
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;
  
  sp -= sizeof *p->tf;
  //分配空间
  p->tf = (struct trapframe*)sp;
  
  // Set up new context to start executing at forkret,
  // which returns to trapret.
  
  //这一步的作用是什么？
  // sp -= 4;
  // *(u_int*)sp = (u_int)trapret;  //陷入返回

  //分配上下文空间
  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  // p->context->eip = (u_int)forkret;

  return p;
}

void user_process_test() {
  printf("user_process_test\n");
  while (1);
}


// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char uinit[], enduinit[];
  // printf("%x -- %x\n", uinit, enduinit);
  
  p = allocproc(); //分配进程槽
  initproc = p;
  if((p->pgdir = setupkpg_t()) == 0)  //分配页目录
    panic("userinit: out of memory?");

  inituvm(p->pgdir, (char*)uinit, (u_int)(enduinit - uinit)); //初始化代码空间
  // curpgdir = p->pgdir;
  // u_int* add = 0;
  // printf("uvm -> %x\n", *add);
  // printf("uvm -> %x\n", *(add + 1));
  // printf("uvm -> %x\n", *(add + 2));

  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));

  p->tf->cp0_cause = 0x10001004;
  // p->tf->regs[29] = USTACKTOP;
  p->tf->regs[29] = 0; //sp
  p->tf->regs[31] = 0; //ra
  //cr3有什么作用？
  p->cr3 = (u_int)p->pgdir - 0x80000000;
  p->tf->pc = 0;

  safestrcpy(p->name, "initcode", sizeof(p->name));
//   p->cwd = namei("/");

  p->state = RUNNABLE;
}

// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  printf("scheduler\n");

  for(;;){
    //开中断
    sti();
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      //切换到选择的进程 需要进入选择进程内释放ptable.lock
      //回到scheduler前再锁住

      curproc = p;
      curtf = p->tf;
      
      switchuvm(p);
      p->state = RUNNING;
      swtch(&scheduler_tf, p->tf);
      //保存调度器上下文 scheduler_tf 加载进程tf

      //再次切回核心进程
      switchkvm();

    }
    release(&ptable.lock);
  }
}