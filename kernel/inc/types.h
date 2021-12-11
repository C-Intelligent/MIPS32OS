#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef NULL
#define NULL ((void*) 0)
#endif

#define STDIN 0
#define STDOUT 1


#define NPROC        64  // maximum number of processes
#define KSTACKSIZE 4096  // size of per-process kernel stack
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NBUF         10  // size of disk block cache
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define LOGSIZE      10  // max data sectors in on-disk log

// Explicitly-sized versions of integer types
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef unsigned long   u_long;
typedef unsigned int    u_int;
typedef unsigned short  u_short;
typedef unsigned char   u_char;
typedef unsigned long long 		u_ll;
typedef u_int pde_t;
typedef u_int pte_t;

// Pointers and addresses are 32 bits long.
// We use pointer types to represent virtual addresses,
// uintptr_t to represent the numerical values of virtual addresses,
// and physaddr_t to represent physical addresses.
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
typedef uint32_t physaddr_t;

// Page numbers are 32 bits long.
typedef uint32_t ppn_t;

// size_t is used for memory object sizes.
typedef uint32_t size_t;

#define MAX_ORDER 1024

// Efficient min and max operations
#define MIN(_a, _b)						    \
({								            \
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a <= __b ? __a : __b;					\
})
#define MAX(_a, _b)						    \
({								            \
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a >= __b ? __a : __b;					\
})

// Rounding operations (efficient when n is a power of 2)
#define ROUND(a, n)     (((((u_long)(a)) + (n) - 1)) & ~((n) - 1))
// Round down to the nearest multiple of n
#define ROUNDDOWN(a, n)						\
({								            \
	uint32_t __a = (uint32_t) (a);			\
	(typeof(a)) (__a - __a % (n));			\
})
// Round up to the nearest multiple of n
#define ROUNDUP(a, n)						                \
({								                            \
	uint32_t __n = (uint32_t) (n);				            \
	(typeof(a)) (ROUNDDOWN((uint32_t) (a) + __n - 1, __n));	\
})

#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

// Return the offset of 'member' relative to the beginning of a struct type
#ifndef offsetof
#define offsetof(type, member)  ((size_t) (&((type*)0)->member))
#endif


// Mutual exclusion lock.
struct spinlock {
  u_int locked;       // Is the lock held?
  
  // For debugging:
  char *name;        // Name of lock.
  u_int pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

struct buf;
struct context;
struct file;
struct inode;
struct pipe;


// 未使用态、初始态、等待态、就绪态、运行态、僵尸态
enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
//进程控制块
struct proc {
  u_int sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  char *ksp;
  enum procstate state;        // Process state
  enum procstate laststate; 
  volatile int pid;            // Process ID
  u_int asid;
  struct proc *parent;         // Parent process

  struct trapframe *tf;        // Trap frame for current syscall
  struct trapframe *context;     // 用于调度

  //是否应该放在这个位置还待定
  u_int cr3;

  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  // struct inode *cwd;           // Current directory
  char* cwd;
  char name[32];               // Process name (debugging)
};

struct _m_stat {
	int v;
};

struct superblock {
	int v;
};


#define CWDPATH_MAX 128



#define PIPESIZE 512

struct pipe {
  struct spinlock lock;
  char data[PIPESIZE];
  struct proc *fa;  //father
  struct proc *chi; //child
  u_int nread;     // number of bytes read
  u_int nwrite;    // number of bytes written
  int readopen;   // read fd is still open
  int writeopen;  // write fd is still open
};