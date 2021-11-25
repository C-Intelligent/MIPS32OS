#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/mips/mips32.h"
#include "../inc/mmu.h"
#include "../inc/printf.h"
#include "../inc/file.h"

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
extern struct trapframe *cur_context;
extern void load_code(pde_t *pgdir, const char* path);
extern pde_t *curpgdir; 

//调度器现场
struct trapframe scheduler_tf;
extern struct trapframe* scheduler_tf_ptr;

extern void tlb_out(u_int va);



void
process_init(void)
{
  initlock(&ptable.lock, "ptable");
  scheduler_tf_ptr = &scheduler_tf;
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

  p->firstcall = 1;

  //初始化输入输出流
  p->ofile[0] = get_std_out_f();

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
  p->context = (struct trapframe*)sp;
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
  

  p->sz = PGSIZE;
  //memset(p->tf, 0, sizeof(*p->tf));

  p->context->cp0_cause = 0x10001004;
  // p->tf->regs[29] = USTACKTOP;
  p->context->regs[29] = 0; //sp
  p->context->regs[31] = 0; //ra
  p->context->cp0_epc = 0;
  //cr3有什么作用？
  p->cr3 = (u_int)p->pgdir - 0x80000000;
  p->context->pc = 0;
  p->context->cp0_status = 0x00007c00;

  //v1放返回地址 -->经验证不适用
  //p->context->regs[3] = 0;

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
  scheduler_tf_ptr = &scheduler_tf;
  
  for(;;){
    //开中断
    //sti();
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;
      
      //切换到选择的进程 需要进入选择进程内释放ptable.lock
      //回到scheduler前再锁住
      //cli();

      curproc = p;
      curtf = p->tf;
      cur_context = p->context;
      
      switchuvm(p);
      p->state = RUNNING;
      //切换前要保存 kernel_sp
      //由于不支持异常嵌套，这里事先填写tlb(若首次分配时间片)
      printf("sche pro :%s  pgdir:%x\n", p->name, p->pgdir);
      
      if (p->firstcall) {
        p->firstcall = 0;
        tlb_out(p->context->regs[31]);
      }
      //保存调度器上下文 scheduler_tf 加载进程tf
      swtchk2u(&scheduler_tf, p->context);
      
      printf("back to scheduler\n");

      //这里要考察进程是否已经销毁
      p->state = RUNNABLE;
      //再次切回核心进程(切回之前要变更状态)
      switchkvm();
      curproc = 0;
      curtf = 0;
      cur_context = 0;
    }
    release(&ptable.lock);
  }
}

int
fork(void)
{
  int i, pid;
  struct proc *np;

  // Allocate process.
  if((np = allocproc()) == 0)
    return -1;

  // Copy process state from p.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }

  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear v0 so that fork returns 0 in the child.
  np->tf->regs[2] = 0;

  //复制文件打开表
  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  
  //cwd分配目录的机制暂未考虑
  //np->cwd = idup(curproc->cwd);
 
  pid = np->pid;
  np->state = RUNNABLE;
  safestrcpy(np->name, curproc->name, sizeof(curproc->name));
  return pid;
}