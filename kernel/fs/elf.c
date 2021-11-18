#include "../inc/elf.h"
#include "../inc/defs.h"
#include "../drivers/ff.h"
#include "../inc/memlayout.h"
#include "../inc/mmu.h"
#include "../inc/string.h"

int load_elf_sd(const uint8_t *elf, const uint32_t elf_size) {
  // sanity checks
  if(elf_size <= sizeof(Elf32_Ehdr))
    return 1;                   /* too small */

  const Elf32_Ehdr *eh = (const Elf32_Ehdr *)elf;
  if(!IS_ELF32(*eh))
    return 2;                   /* not a elf32 file */

  const Elf32_Phdr *ph = (const Elf32_Phdr *)(elf + eh->e_phoff);
  if(elf_size < eh->e_phoff + eh->e_phnum*sizeof(*ph))
    return 3;                   /* internal damaged */
  // uint32_t entry_point = eh->e_entry;
  // printf("elf entry point addr : %x \n", entry_point);
  uint32_t i;
  for(i=0; i<eh->e_phnum; i++) {
    if(ph[i].p_type == PT_LOAD && ph[i].p_memsz) { /* need to load this physical section */
      printf("[load_elf]still alive ... writing %d bytes to ", ph[i].p_filesz);
      //printf("%x \n\r", (uintptr_t)ph[i].p_paddr);
      printf("%x \n\r", (uint32_t)ph[i].p_paddr);
      
      if(ph[i].p_filesz) {                         /* has data */
        if(elf_size < ph[i].p_offset + ph[i].p_filesz)
          return 3;             /* internal damaged */
        memmove((uint8_t *)ph[i].p_paddr, elf + ph[i].p_offset, ph[i].p_filesz);
      }
      if(ph[i].p_memsz > ph[i].p_filesz) { /* zero padding */
        memset((uint8_t *)ph[i].p_paddr + ph[i].p_filesz, 0, ph[i].p_memsz - ph[i].p_filesz);
      }
    }
  }
  return 0;
}

void load_code(pde_t *pgdir, const char* path) {
  FILINFO stat;
  f_stat(path, &stat);

  FIL fpt;
  FRESULT res;
  res = f_open (&fpt, path, FA_READ);

  
  uint32_t br = 0;
  u_int base = 0;

  char* buffer = kalloc();
  res = f_read(&fpt, buffer, 0x1000, &br);

  for (;;) {
    char* buffer = kalloc();
    res = f_read (&fpt, buffer, 0x1000, &br);
    mappages(pgdir, base, PGSIZE, (u_int)buffer, PTE_W|PTE_U);
    base += PGSIZE;
    if (br < 0x1000) break;
  }
  
  printf("%s\n", buffer);

  f_close (&fpt); 

}