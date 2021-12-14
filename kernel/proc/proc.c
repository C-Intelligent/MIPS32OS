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

extern void back2sche();
extern void sche2wake();

void proc2runnable() {
  curproc->state = RUNNABLE;
}

void lock_ptable() {
  acquire(&ptable.lock);
}


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

  //初始化输入输出流
  p->ofile[STDIN] = get_std_in_f();
  p->ofile[STDOUT] = get_std_out_f();

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

  //分配文件目录名称空间
  sp -= CWDPATH_MAX;
  p->cwd = (char*)sp;

  //设置内核栈栈顶（供系统调用使用）
  p->ksp = (char*)sp;
  //printf("kstack top: %x\n", sp);

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
  //printf("user pgdir:%x\n", p->pgdir);

  p->sz = PGSIZE;
  //memset(p->tf, 0, sizeof(*p->tf));

  p->context->cp0_cause = 0x10001004;
  // p->tf->regs[29] = USTACKTOP;
  p->context->regs[29] = 0; //sp
  p->context->regs[31] = 0x100; //ra
  p->context->cp0_epc = 0x100;
  //cr3有什么作用？
  p->cr3 = (u_int)p->pgdir - 0x80000000;
  p->context->pc = 0x100;
  p->context->cp0_status = 0x00007c00;

  //v1放返回地址 -->经验证不适用
  //p->context->regs[3] = 0;

  safestrcpy(p->name, "initcode", sizeof(p->name));
  //进程当前目录：挂在/下
  extern char curcwd[];
  safestrcpy(p->cwd, curcwd, CWDPATH_MAX);
//   p->cwd = namei("/");

  p->state = RUNNABLE;
}

// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
u_int sche_return_addr_v;
u_int from_sche = 0;
extern u_int rounds;

void
scheduler(void)
{
  struct proc *p;
  // printf("scheduler\n");
  scheduler_tf_ptr = &scheduler_tf;
  
  for(;;){
    //开中断
    //sti();
    acquire(&ptable.lock);
    rounds++;
    wakeup_on_train(&rounds);
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
      
      // printf("sche pro :%s ", p->name);
      // printf("  pid: %d\n", p->pid);
      
      // printf("return addr i: %x  ", sche_return_addr_v);
      // printf("return addr: %x  ", p->context->cp0_epc);
      // printf("tf : v0 %d   proc sp: %x \n", p->context->regs[2], p->context->regs[29]);

      from_sche = 1;
      sche_return_addr_v = *(u_int*)(p->context->cp0_epc);
      // u_int* add = (u_int*)(p->context->cp0_epc);
      
      // for (;add < p->context->cp0_epc + 20;add++) {
      //   printf("%x %x \n", add, *add);
      // }

      
      // if (p->sz > 4096) sche_return_addr_v = *(u_int*)(0x00001000);
      
      //保存调度器上下文 scheduler_tf 加载进程tf
      release(&ptable.lock);
      if (p->laststate == SLEEPING) {
        p->laststate = RUNNABLE;
        sche2wake();
      }
      else {
        p->laststate = RUNNABLE;
        swtchk2u(&scheduler_tf, p->context, p->context->cp0_epc);
      }
      acquire(&ptable.lock);
      //printf("back to scheduler\n");

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
  //printf("start to fork: cur pid:%d  ", curproc->pid);
  // printf("from:%x  ra:%x\n", curproc->tf->cp0_epc, curproc->tf->regs[31]);
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
  *np->context = *curproc->tf;
  np->context->cp0_epc += 4;

  //debug
  
  extern char* child_proc_return_add;
  //v1保存子进程返回位置
  //np->context->cp0_epc = child_proc_return_add;
  //np->tf->regs[3] = np->tf->cp0_epc;
  

  // Clear v0 so that fork returns 0 in the child.
  np->tf->regs[2] = 0;
  np->context->regs[2] = 0;

  //复制文件打开表
  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  
  //cwd分配目录的机制暂未考虑
  //np->cwd = idup(curproc->cwd);
  safestrcpy(np->cwd, curproc->cwd, CWDPATH_MAX);
 
  pid = np->pid;
  np->state = RUNNABLE;
  safestrcpy(np->name, curproc->name, sizeof(curproc->name));
  // printf("fnish fork  new pid:%d \n", np->pid);
  return pid;
}

//关闭打开文件，唤醒父进程，移交子进程
//然后进入调度器
//父进程wait()释放资源
void
exit(void)
{
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  //暂未实现cwd
  // proc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup(initproc);
    }
  }

  // 回到调度器
  curproc->state = ZOMBIE;
  release(&ptable.lock);
  back2sche();
  //sched();
  panic("zombie exit");
}


// Wake up  
void
wakeup(struct proc *p)
{
  if(p->state == SLEEPING)
    p->state = RUNNABLE;
}

//等待子进程结束 然后回收资源
int
wait(void)
{
  struct proc *p;
  int havekids, pid;

  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for zombie children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->state = UNUSED;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  
    // sleep(proc, &ptable.lock);  //DOC: wait-sleep
    sleep(curproc);
  }
}


//简化版本睡眠
//等待被唤醒
//sleep调用之前需上锁
void
sleep(struct proc *p)
{
  if(p == 0)
    panic("sleep");

  p->state = SLEEPING;
  p->laststate = SLEEPING;

  release(&ptable.lock);
  back2sche();
}

void
sleep_on_train(void *chan)
{
  if(curproc == 0)
    panic("sleep");

  curproc->state = SLEEPING;
  curproc->laststate = SLEEPING;
  curproc->chan = chan;

  release(&ptable.lock);
  back2sche();
}

void 
wakeup_on_train(void *chan) {
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

int
growproc(int n)
{
  u_int sz;
  
  sz = curproc->sz;

  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  return 0;
}

//打印进程信息
int
disprocs(void) {
  struct proc *p;
  printf("PID   size   STATE\n");
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED) continue;
    printf("%d     %x", p->pid, p->sz);
    switch (p->state)
    {
    case EMBRYO:
      printf("   EMBRYO\n");
      break;
    case SLEEPING:
      printf("   SLEEPING\n");
      break;
    case RUNNABLE:
      printf("   RUNNABLE\n");
      break;
    case RUNNING:
      printf("   RUNNING\n");
      break;
    case ZOMBIE:
      printf("   ZOMBIE\n");
      break;
    
    default:
      break;
    }
  }
  return 0;
}