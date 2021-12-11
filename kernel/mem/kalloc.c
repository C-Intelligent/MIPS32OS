#include "../inc/types.h"
#include "../inc/defs.h"
// #include "param.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"
#include "../inc/printf.h"
#include "../inc/string.h"

void freerange(void *vstart, void *vend);

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;  //存储线性映射地址(无需经过tlb 仅内核态可访问)
} kmem;


void
phys_page_allocator_init(void *start, void *end)
{
  printf("end addr:%x\n", end);
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  kmem.freelist = NULL;
  freerange(start, end);
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((u_int)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
void
kfree(char *v)
{
  if (v < 0x80000000) panic("wrong page: %x to free", v);
  struct run *r;

//   if((u_int)v % PGSIZE || v < end || v2p(v) >= PHYSTOP)
//     panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;  //插到链头
  kmem.freelist = r;
  // printf("kfree %x last %x\n", r, r->next);
  if(kmem.use_lock)
    release(&kmem.lock);
}

char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  printf("kalloc: %x next: %x\n", r, r->next);
  if(r)
    kmem.freelist = r->next;
  if(kmem.use_lock)
    release(&kmem.lock);
  
  memset(r, 0, PGSIZE);
  
  *(int*)r = 0;
  return (char*)r;
}
