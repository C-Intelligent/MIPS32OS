#pragma once
#include "types.h"
#include "../drivers/ff.h"
#include "file.h"

// exec.c
int             exec(char*, int, char**);

// file.c
void fs_init();
struct file*    get_std_out_f(); 
struct file*    get_std_in_f();
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
FNODE * fnodealloc(void);
int             filestat(struct file*, struct stat*);
int             filewrite(struct file*, char*, int n);
int             fileread(struct file*, char*, int n);

void print_startimg();

int chdir(const char *path);
void pwd();
void ls();

FNODE * namef(const char* path);

// kalloc.c
char*           kalloc(void);
void            kfree(char*);
void            phys_page_allocator_init(void*, void*);



// pipe.c
int             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, int);
int             piperead(struct pipe*, char*, int);
int             pipewrite(struct pipe*, char*, int);

//PAGEBREAK: 16
// proc.c
void lock_ptable();
struct proc*    copyproc(struct proc*);
void            exit(void);
int             fork(void);
int             growproc(int);
// int             kill(int);
void            process_init(void);  //进程初始化
// void            procdump(void);
void            scheduler(void) __attribute__((noreturn)); //调度器
void sleep(struct proc *p);
void            userinit(void);
int             wait(void);
void wakeup(struct proc *p);
void wakeup_on_train(void *chan);
void sleep_on_train(void *chan);

// swtch.S
void            swtchk2u(struct trapframe* save_to, struct trapframe * to_load, u_int);

// spinlock.c
void            acquire(struct spinlock*);
void            getcallerpcs(void*, u_int*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            pushcli(void);
void            popcli(void);

// syscall.c


// timer.c
void            timer_init(void);

// trap.c
void trap_init();


extern struct spinlock tickslock;

// vm.c
u_int           searchPN(void* addr);
void allocate8KB(u_int *bada, u_int *arr, u_int epc);
void            init_kpg_table(void); //为核心创建页表
pde_t*          setupkpg_t(void);  //创建页表
char*           uva2ka(pde_t*, char*);

//为用户分配虚拟空间
int             allocuvm(pde_t*, u_int, u_int);
int             deallocuvm(pde_t*, u_int, u_int);

int loaduvm(pde_t *pgdir, char *addr, FIL *fp, u_int offset, u_int sz);

void            freevm(pde_t*);
void            inituvm(pde_t*, char*, u_int);  //初始化用户进程页表
pde_t*          copyuvm(pde_t*, u_int);
void            switchuvm(struct proc*);
void            switchkvm(void); //切换到内核页表
int             copyout(pde_t*, u_int, void*, u_int);
void            clearpteu(pde_t *pgdir, char *uva);




// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))



