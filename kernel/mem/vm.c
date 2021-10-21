#include "../inc/types.h"
#include "../inc/defs.h"
// #include "param.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"
#include "../inc/string.h"
#include "../inc/printf.h"

pde_t *kpgdir;  // for use in scheduler()
// struct segdesc gdt[NSEGS];

// This table defines the kernel's mappings, which are present in
// every process's page table.
static struct kmap {
  void *vir_addr;
  u_int phys_start;
  u_int phys_end;
  int perm;   //权限 
} kmap[] = {  //核心从0x80000000开始
 { (void*)KERNEL_BASE, KERNEL_BASE,   KERNEL_TEXT_START,    PTE_W}, // exception space
 { (void*)KERNEL_TEXT_START, KERNEL_TEXT_START, KERNEL_ELF_END, 0},     // kern text+rodata data+memory
//  { (void*)data,     V2P(data),     PHYSTOP,   PTE_W}, // kern data+memory
//  { (void*)DEVSPACE, DEVSPACE,      0,         PTE_W}, // more devices
};

// Allocate one page table for the machine for the kernel address
// space for scheduler processes.
void
init_kpg_table(void)
{
  kpgdir = setupkpg_t();
  switchkvm();
}

// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
// 申请物理页，创建页表条目
// 为一个虚拟地址寻找 PTE 若未找到则分配(alloc==0)
static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc)
{
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];  //通过前十位(页目录)(偏移)寻找页表(地址)

  //pde解引用为页表线性地址

  if(*pde & PTE_P){
    pgtab = (pte_t*)PTE_ADDR(*pde);  //去掉末尾标志
  } else {
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
      return 0;
    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table 
    // entries, if necessary.
    *pde = (u_int)pgtab | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)]; //通过中十位(页表索引)找到页表条目地址
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned.
// 在页表中建立一段虚拟内存到一段物理内存的映射
/*注：起始地址必须规范给出（4K对齐）*/
static int
mappages(pde_t *pgdir, void *va, u_int size, u_int pa, int perm)
{
  char *a, *last;
  pte_t *pte;
  
  a = (char*)PGROUNDDOWN((u_int)va);
  last = (char*)PGROUNDDOWN(((u_int)va) + size - 1);
  for(;;){
    if((pte = walkpgdir(pgdir, a, 1)) == 0)
      return -1;
    if(*pte & PTE_P)  //此地址已经映射过
      panic("remap");
    
    *pte = pa | perm | PTE_P;   //由于这些地址是4K对齐地址  所以末12位可以用来存放权限信息

    if(a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

// Set up kernel part of a page table.
pde_t*
setupkpg_t(void)
{
  pde_t *pgdir;
  struct kmap *k;

  if((pgdir = (pde_t*)kalloc()) == 0)
    return 0;
  memset(pgdir, 0, PGSIZE);

  for(k = kmap; k < &kmap[NELEM(kmap)]; k++)
    if(mappages(pgdir, k->vir_addr, k->phys_end - k->phys_start, 
                (u_int)k->phys_start, k->perm) < 0)
      return 0;
  return pgdir;
}

void
switchkvm(void)
{
//   lcr3(v2p(kpgdir));   // switch to the kernel page table
}

// Load the initcode into address 0 of pgdir.
// sz must be less than a page.
void
inituvm(pde_t *pgdir, char *init, u_int sz)
{
  char *mem;
  
  if(sz >= PGSIZE)
    panic("inituvm: more than a page");
  mem = kalloc();
  memset(mem, 0, PGSIZE);
  mappages(pgdir, 0, PGSIZE, (u_int)mem, PTE_W|PTE_U);
  memmove(mem, init, sz);
}