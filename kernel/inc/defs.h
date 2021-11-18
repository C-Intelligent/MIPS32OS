#pragma once
#include "types.h"
#include "../drivers/ff.h"

// bio.c
// void            binit(void);
// struct buf*     bread(u_int, u_int);
// void            brelse(struct buf*);
// void            bwrite(struct buf*);

// console.c
// void            consoleinit(void);
// void            cprintf(char*, ...);
// void            consoleintr(int(*)(void));
// void            panic(char*) __attribute__((noreturn));

// exec.c
// int             exec(char*, char**);
int             exec(char*, int, char**);

// file.c
// struct file*    filealloc(void);
// void            fileclose(struct file*);
// struct file*    filedup(struct file*);
// void            fileinit(void);
// int             fileread(struct file*, char*, int n);
// int             filestat(struct file*, struct _m_stat*);
// int             filewrite(struct file*, char*, int n);

// fs.c
// void            readsb(int dev, struct superblock *sb);
// int             dirlink(struct inode*, char*, u_int);
// struct inode*   dirlookup(struct inode*, char*, u_int*);
// struct inode*   ialloc(u_int, short);
// struct inode*   idup(struct inode*);
// void            iinit(void);
// void            ilock(struct inode*);
// void            iput(struct inode*);
// void            iunlock(struct inode*);
// void            iunlockput(struct inode*);
// void            iupdate(struct inode*);
// int             namecmp(const char*, const char*);
// struct inode*   namei(char*);
// struct inode*   nameiparent(char*, char*);
// int             readi(struct inode*, char*, u_int, u_int);
// void            stati(struct inode*, struct _m_stat*);
// int             writei(struct inode*, char*, u_int, u_int);

// ide.c
// void            ideinit(void);
// void            ideintr(void);
// void            iderw(struct buf*);

// ioapic.c
// void            ioapicenable(int irq, int cpu);
// extern u_char    ioapicid;
// void            ioapicinit(void);

// kalloc.c
char*           kalloc(void);
void            kfree(char*);
void            phys_page_allocator_init(void*, void*);


// kbd.c
// void            kbdintr(void);

// lapic.c
// int             cpunum(void);
// extern volatile u_int*    lapic;
// void            lapiceoi(void);
// void            lapicinit(void);
// void            lapicstartap(u_char, u_int);
// void            microdelay(int);

// log.c
// void            initlog(void);
// void            log_write(struct buf*);
// void            begin_trans();
// void            commit_trans();

// mp.c
// extern int      ismp;
// int             mpbcpu(void);
// void            mpinit(void);
// void            mpstartthem(void);

// picirq.c
// void            picenable(int);
// void            picinit(void);

// pipe.c
// int             pipealloc(struct file**, struct file**);
// void            pipeclose(struct pipe*, int);
// int             piperead(struct pipe*, char*, int);
// int             pipewrite(struct pipe*, char*, int);

//PAGEBREAK: 16
// proc.c
struct proc*    copyproc(struct proc*);
// void            exit(void);
int             fork(void);
int             growproc(int);
int             kill(int);
void            process_init(void);  //进程初始化
void            procdump(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
// void            sleep(void*, struct spinlock*);
void            userinit(void);
int             wait(void);
void            wakeup(void*);
void            yield(void);

// swtch.S
void            swtch(struct trapframe* save_to, struct trapframe * to_load);

// spinlock.c
void            acquire(struct spinlock*);
void            getcallerpcs(void*, u_int*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            pushcli(void);
void            popcli(void);

// // string.c
// int             memcmp(const void*, const void*, u_int);
// void*           memmove(void*, const void*, u_int);
// void*           memset(void*, int, u_int);
// char*           safestrcpy(char*, const char*, int);
// int             strlen(const char*);
// int             strncmp(const char*, const char*, u_int);
// char*           strncpy(char*, const char*, int);

// syscall.c
int             argint(int, int*);
int             argptr(int, char**, int);
int             argstr(int, char**);
int             fetchint(u_int, int*);
int             fetchstr(u_int, char**);

// timer.c
void            timer_init(void);

// trap.c
void trap_init();

void            idtinit(void);
extern u_int     ticks;
void            tvinit(void);
extern struct spinlock tickslock;

// vm.c
u_int searchPN(void* addr);
void allocate8KB(u_int *entryHi, u_int *pa);
void            seginit(void);
void            init_kpg_table(void); //为核心创建页表
void            vmenable(void);
pde_t*          setupkpg_t(void);  //设置核心页表
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


//fs_init.c
void fs_init();

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

// #define KERNBASE 0x80010000

// #define VPT (ULIM + PDMAP )
// #define KSTACKTOP (VPT-0x100)
// #define KSTKSIZE (8*BY2PG)
// #define ULIM 0x80000000

// #define UVPT (ULIM - PDMAP)
// #define UPAGES (UVPT - PDMAP)
// #define UENVS (UPAGES - PDMAP)

// #define UTOP UENVS
// #define UXSTACKTOP (UTOP)
// #define TIMESTACK 0x82000000

// #define USTACKTOP (UTOP - 2*BY2PG)
// #define UTEXT 0x00400000


