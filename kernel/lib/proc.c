#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/mips/mips32.h"
#include "../inc/mmu.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;
int nextpid = 1;

extern void trapret(void);

// current process.
extern struct proc *proc asm("%gs:4");     // cpus[cpunum()].proc

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


// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
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
  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;
  
  sp -= sizeof *p->tf;
//   sp -= sizeof(struct trapframe);
  p->tf = (struct trapframe*)sp;
  
  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(u_int*)sp = (u_int)trapret;  //陷入返回

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (u_int)forkret;

  return p;
}


// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];
  
  p = allocproc(); //手动分配进程
  initproc = p;
  if((p->pgdir = setupkpg_t()) == 0)  //分配页目录
    panic("userinit: out of memory?");

// 这是哪里来的？
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size); //初始化页表

  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
//   p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
//   p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
//   p->tf->es = p->tf->ds;
//   p->tf->ss = p->tf->ds;
//   p->tf->eflags = FL_IF;
//   p->tf->esp = PGSIZE;
//   p->tf->eip = 0;  // beginning of initcode.S

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

  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      proc = p;
      switchuvm(p);
      p->state = RUNNING;
    //   swtch(&cpu->scheduler, proc->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      proc = 0;
    }
    release(&ptable.lock);

  }
}