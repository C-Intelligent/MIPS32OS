#include "../inc/mfp_io.h"
#include "../inc/printf.h"
#include "../inc/types.h"
#include "../inc/defs.h"
#include "../inc/memlayout.h"

extern char end[]; // first address after kernel loaded from ELF file

int 
main(void) {
  printf("\nHello Operating System!\n");
  printf("Hello Operating System!\n");
  printf("Hello Operating System!\n");
  
  phys_page_allocator_init(end, P2V(4*1024*1024));  // phys page allocator



  while (1);

  return 0;
}