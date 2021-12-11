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
  struct cpu *cpu;   // The cpu holding the lock.
  u_int pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

#define EXEC  1   //执行
#define REDIR 2   //重定向
#define PIPE  3   //管道
#define LIST  4   //参数列表
#define BACK  5

#define MAXARGS 10

struct cmd {
  int type;
};

struct execcmd {
  int type;
  int argc;
  char *argv[MAXARGS];
  char *eargv[MAXARGS];
};

struct redircmd {
  int type;
  struct cmd *cmd;
  char *file;
  char *efile;
  int mode;
  int fd;
};

struct pipecmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

struct listcmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

struct backcmd {
  int type;
  struct cmd *cmd;
};

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200


