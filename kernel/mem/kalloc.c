#include "../inc/types.h"
#include "../inc/defs.h"
// #include "param.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;


void
phys_page_allocator_init(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  kmem.freelist = NULL;
  freerange(vstart, vend);
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
  struct run *r;

//   if((u_int)v % PGSIZE || v < end || v2p(v) >= PHYSTOP)
//     panic("kfree");

  // Fill with junk to catch dangling refs.
//   memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;  //插到链头
  kmem.freelist = r;
  if(kmem.use_lock)
    release(&kmem.lock);
}
