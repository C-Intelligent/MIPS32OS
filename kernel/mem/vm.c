#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"
#include "../inc/string.h"
#include "../inc/printf.h"
#include "../drivers/ff.h"


pde_t *kpgdir, *curpgdir;  // for use in scheduler()
extern u_int curasid;

// struct segdesc gdt[NSEGS];

// This table defines the kernel's mappings, which are present in
// every process's page table.
static struct kmap {
  void *vir_addr;
  u_int phys_start;
  u_int phys_end;
  int perm;   //权限 
} kmap[] = {  //核心从0x80000000开始
// { (void*)KERNEL_BASE, KERNEL_BASE,   KERNEL_TEXT_START,    PTE_W}, // exception space
// { (void*)KERNEL_TEXT_START, KERNEL_TEXT_START, KERNEL_ELF_END, 0},     // kern text+rodata data+memory
//  { (void*)data,     V2P(data),     PHYSTOP,   PTE_W}, // kern data+memory
//  { (void*)DEVSPACE, DEVSPACE,      0,         PTE_W}, // more devices
};

// Allocate one page table for the machine for the kernel address
// space for scheduler processes.
void
init_kpg_table(void)
{
  kpgdir = setupkpg_t();
  curpgdir = kpgdir;
  switchkvm();
}



// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
// 申请物理页，创建页表条目
// 为一个虚拟地址寻找 PTE 若未找到则分配(alloc==1)
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
//警告：若不对齐可能会产生严重错误
//警告：若配合kalloc使用，一次只能分配一页，否则会产生严重错误
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
      panic("remap: %x", pte);
    
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
  curpgdir = kpgdir;
  curasid = 0;
//   lcr3(v2p(kpgdir));   // switch to the kernel page table
}

extern void change_entryhi();

// Switch page table to process p.
void
switchuvm(struct proc *p)
{
  pushcli();
  // cpu->gdt[SEG_TSS] = SEG16(STS_T32A, &cpu->ts, sizeof(cpu->ts)-1, 0);
  // cpu->gdt[SEG_TSS].s = 0;
  // cpu->ts.ss0 = SEG_KDATA << 3;
  // cpu->ts.esp0 = (uint)proc->kstack + KSTACKSIZE;
  // ltr(SEG_TSS << 3);
  // if(p->pgdir == 0)
  //   panic("switchuvm: no pgdir");
  // lcr3(v2p(p->pgdir));  // switch to new address space

  curpgdir = p->pgdir;
  curasid = p->asid;

  change_entryhi();

  //printf("curpgdir:%x\n", curpgdir);
  popcli();
}

// Load the initcode into address 0 of pgdir.
// sz must be less than a page.
void
inituvm(pde_t *pgdir, char *init, u_int sz)
{
  char *mem;
  
  if(sz +  0x100 >= PGSIZE)
    panic("inituvm: more than a page");
  mem = kalloc();
  //kfree(mem);
  // mem = kalloc();
  memset(mem, 0, PGSIZE);
  mappages(pgdir, 0, PGSIZE, (u_int)mem, PTE_W|PTE_U);
  memmove(mem + 0x100, init, sz);
  
  u_int* add = mem;
  
  // for (;add < mem + 25;add++) {
  //   printf("%x  %x\n", add, *add);
  // }
  
}



//由于tlb每次重填两项 故需确认页表填好对应项
void allocate8KB(u_int *bada, u_int *arr, u_int epc) {
  u_int entryHi = (u_int)bada & 0xffffe000;

  pte_t *pte = walkpgdir(curpgdir, entryHi, 1); //找到条目
  u_int pa = *pte;
  if (pa == 0) {
    pa = (u_int)kalloc();
    mappages(curpgdir, (u_int*)entryHi, PGSIZE, pa, PTE_W|PTE_U);
  }
  pa = pa & 0xfffff000;
  arr[0] = pa - 0x80000000;
  
  entryHi += PGSIZE;

  pte = walkpgdir(curpgdir, entryHi, 1); //找到条目
  pa = *pte;
  if (pa == 0) {
    pa = (u_int)kalloc();
    mappages(curpgdir, (u_int *)entryHi, PGSIZE, pa, PTE_W|PTE_U);
  }
  pa = pa & 0xfffff000;
  arr[1] = pa - 0x80000000; 
  printf("bada: %x  epc: %x pa0 %x   pa1  %x\n", bada, epc, arr[0], arr[1]);
  /*
  printf("entryHi: %x  pa0 %x   pa1%x\n", entryHi, arr[0], arr[1]);
  u_int* add = (u_int*)((u_int)arr[0] + 0x80000000);
  for (;add < ((u_int)arr[0] + 0x80000000 + 40);add++) {
    printf("%x  %x\n", add, *add);
  }
  */
}


//@已弃用
//查找虚拟地址对应的物理页号
u_int searchPN(void* addr) {
  pte_t *pte = walkpgdir(curpgdir, addr, 1); //找到条目
  u_int pa = *pte;
  if (pa == 0) {
    pa = (u_int)kalloc();
    mappages(curpgdir, addr, PGSIZE, pa, PTE_W|PTE_U);
  }
  return pa - 0x80000000;
}

// Clear PTE_U on a page. Used to create an inaccessible
// page beneath the user stack.
void
clearpteu(pde_t *pgdir, char *uva)
{
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
  if(pte == 0)
    panic("clearpteu");
  *pte &= ~PTE_U;
}

//将程序加载到线性空间(虚拟空间)
int
loaduvm(pde_t *pgdir, char *addr, FIL *fp, u_int offset, u_int sz)
{
  u_int i, pa, n;
  pte_t *pte;
  FRESULT res;

  uint32_t oldptr = fp->fptr;
  // printf("old file ptr: %u\n", oldptr);
  res = f_lseek(fp, offset);
  // printf("new file ptr: %u\n", fp->fptr);


  if((u_int) addr % PGSIZE != 0)
    panic("loaduvm: addr must be page aligned");

  for(i = 0; i < sz; i += PGSIZE){
    
    if((pte = walkpgdir(pgdir, addr+i, 0)) == 0)
      panic("loaduvm: address should exist");
    pa = PTE_ADDR(*pte); //物理页框首地址
    printf("[loaduvm] va: %x pa: => %x\n", addr+i , pa);
    if(sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;
    
    uint32_t br = 0;
    res = f_read(fp, (void*)pa, n, &br);
    if (br != n) return -1;
  }
  res = f_lseek(fp, oldptr);
  return 0;
}


//增长虚存空间
int
allocuvm(pde_t *pgdir, u_int oldsz, u_int newsz)
{
  char *mem;
  u_int a;

  if(newsz >= KERNEL_BASE)
    return 0;
  if(newsz < oldsz)
    return oldsz;

  a = PGROUNDUP(oldsz); //向上取整

  for(; a < newsz; a += PGSIZE){
    mem = kalloc();
    if(mem == 0){
      printf("allocuvm out of memory\n");
      deallocuvm(pgdir, newsz, oldsz);
      return 0;
    }
    // memset(mem, 0, PGSIZE);
    printf("[allocuvm]  va: %x => pa: %x\n", a, mem);
    mappages(pgdir, (void*)a, PGSIZE, (u_int)mem, PTE_W|PTE_U);
  }
  return newsz;
}

int
deallocuvm(pde_t *pgdir, u_int oldsz, u_int newsz)
{
  pte_t *pte;
  u_int a, pa;

  if(newsz >= oldsz)
    return oldsz;

  a = PGROUNDUP(newsz);
  for(; a  < oldsz; a += PGSIZE){
    pte = walkpgdir(pgdir, (char*)a, 0);
    if(!pte)
      a += (NPTENTRIES - 1) * PGSIZE;
    else if((*pte & PTE_P) != 0){
      pa = PTE_ADDR(*pte);
      if(pa == 0)
        panic("kfree");
      char *v = (char*)pa;
      kfree(v);
      *pte = 0;
    }
  }
  return newsz;
}

//复制len长度的字节到虚拟地址空间 p:内核空间，线性地址
int
copyout(pde_t *pgdir, u_int va, void *p, u_int len)
{
  char *buf, *pa0;
  u_int n, va0; //va0:虚拟页首地址

  buf = (char*)p;
  while(len > 0){
    va0 = (u_int)PGROUNDDOWN(va);
    pa0 = uva2ka(pgdir, (char*)va0);  //pa0线性首地址
    if(pa0 == 0)
      return -1;
    n = PGSIZE - (va - va0); //
    if(n > len)
      n = len;
    memmove(pa0 + (va - va0), buf, n);
    len -= n;
    buf += n;
    va = va0 + PGSIZE;
  }
  return 0;
}


// 根据虚拟地址获取线性页首地址
char*
uva2ka(pde_t *pgdir, char *uva)
{
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
  if((*pte & PTE_P) == 0)
    return 0;
  if((*pte & PTE_U) == 0)
    return 0;
  return (char*)(*pte & ~0xfff);
}

//释放虚拟空间
void
freevm(pde_t *pgdir)
{
  u_int i;

  if(pgdir == 0)
    panic("freevm: no pgdir");
  deallocuvm(pgdir, KERNEL_BASE, 0);

  for(i = 0; i < NPDENTRIES; i++){
    if(pgdir[i] & PTE_P){
      char * v = (char*)PTE_ADDR(pgdir[i]);
      kfree(v);
    }
  }
  kfree((char*)pgdir);
}

//复制父进程的虚存空间
pde_t*
copyuvm(pde_t *pgdir, u_int sz)
{
  pde_t *d;
  pte_t *pte;
  u_int pa, i;
  char *mem;

  if((d = setupkpg_t()) == 0)
    return 0;
  
  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walkpgdir(pgdir, (void *) i, 0)) == 0)
      panic("copyuvm: pte should exist");
    if(!(*pte & PTE_P))
      panic("copyuvm: page not present");
    pa = PTE_ADDR(*pte);
    if((mem = kalloc()) == 0)
      goto bad;
    
    memmove(mem, (char*)pa, PGSIZE);

    if(mappages(d, (void*)i, PGSIZE, mem, PTE_W|PTE_U) < 0)
      goto bad;
  }
  return d;

bad:
  freevm(d);
  return 0;
}