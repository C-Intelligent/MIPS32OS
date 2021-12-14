#include "../inc/defs.h"
#include "../inc/types.h"
#include "../inc/elf.h"
#include "../drivers/ff.h"
#include "../inc/printf.h"
#include "../inc/mmu.h"
#include "../inc/string.h"
#include "../inc/mips/mips32.h"
#include "../inc/file.h"

u_int after_exec = 0;
extern struct proc *curproc;

int
exec(char *path, int argc, char **argv)
{
    // printf("exec! path add: %x  argc:%x   argv:%x \n", path, argc, argv);
    // printf("exec : %s\n", path);
    char *s, *last;

    u_int sz, sp;
    u_int ustack[3+MAXARG+1];

    pde_t *pgdir, *oldpgdir;

    pgdir = 0;

    // printf("stdout file: %x, writable:%x\n", curproc->ofile[STDOUT], curproc->ofile[STDOUT]->writable);

    //elf文件解析
    char* buffer = kalloc();
    char* ptr = buffer;
    Elf32_Ehdr *Elf_header = (Elf32_Ehdr *)ptr;
    ptr += sizeof(Elf32_Ehdr);
    Elf32_Shdr *Section_header = (Elf32_Shdr *)ptr;
    ptr += sizeof(Elf32_Shdr);
    Elf32_Sym  *Symbol_tab = (Elf32_Sym  *)ptr;
    ptr += sizeof(Elf32_Sym);
    FIL *fp = (FIL *)ptr;
    ptr += sizeof(FIL);

    char fpath[128];
    safestrcpy(fpath, path, sizeof(fpath));
    FRESULT res = f_open(fp, fpath, FA_READ);
    //如果本地找不到  再到bin中找
    if (FR_OK != res) {
        //printf("res state:%d\n", res);
        safestrcpy(fpath, "0:/bin/", 8);
        safestrcpy(&fpath[7], path, sizeof(fpath) - 8);
        // printf("file: %s\n", fpath);
        res = f_open(fp, fpath, FA_READ);
        if (FR_OK != res) {
            printf("cannot find file: %s\n", fpath);
            goto bad;
        }
    }
    uint32_t br = 0;

    /*文件大小*/
    FILINFO stat;
    f_stat(fpath, &stat);
    //文件太小也会错误
    if(stat.fsize <= sizeof(Elf32_Ehdr)) {
        printf("fsize too small\n");
        goto bad;
    }
        

    //文件头
    f_read(fp, Elf_header, sizeof(Elf32_Ehdr), &br);
    if(!IS_ELF32(*Elf_header)) {
        printf("not a elf file\n");
        goto bad;
    }
        
    
    Elf32_Phdr *Pro_header = (Elf32_Phdr *)ptr;
    ptr += Elf_header->e_phnum * sizeof(Elf32_Phdr);
    //程序头
    u_int phnum = Elf_header->e_phnum;
    //printf("[load_elf] pro_header num :%u\n", phnum);
    br = 0;
    f_read(fp, Pro_header, Elf_header->e_phnum * sizeof(Elf32_Phdr), &br);
    if(stat.fsize < Elf_header->e_phoff + Elf_header->e_phnum*sizeof(Elf32_Phdr)) {
        printf("fsize smaller than e_phoff + e_phnum\n");
        goto bad;
    }
        

    //设置新页表
    if((pgdir = setupkpg_t()) == 0) {
        printf("fail to setupkpg_t\n");
        goto bad;
    }
        

    sz = 0;//准备分配物理页框

    //加载程序段到线性内存
    u_int i;
    for(i = 0; i < Elf_header->e_phnum; i++) {
        if(Pro_header[i].p_type == PT_LOAD && Pro_header[i].p_memsz) { 
            // printf("[load_elf] still alive ... writing %d bytes to pa:", Pro_header[i].p_filesz);
            // printf("%x va:%x\n\r", Pro_header[i].p_paddr, Pro_header[i].p_vaddr);
            /* 此段大小大于0且需要加载 */

            if(Pro_header[i].p_filesz) {                         /* has data */
                if(stat.fsize < Pro_header[i].p_offset + Pro_header[i].p_filesz)
                    goto bad;

                // printf("[load_elf] pro size:%u  mem size: %u\n", Pro_header[i].p_filesz, Pro_header[i].p_memsz);
                // printf("[load_elf] pro offset:%u\n", Pro_header[i].p_offset);
                //分配内存 load  sz:old_size
                if((sz = allocuvm(pgdir, sz, Pro_header[i].p_vaddr + Pro_header[i].p_memsz)) == 0)
                    goto bad;
                    
                if(loaduvm(pgdir, (char*)Pro_header[i].p_vaddr, fp, 
                    Pro_header[i].p_offset, Pro_header[i].p_filesz) < 0)
                    goto bad;
            }
            //不做0扩展会出什么问题？
            if(Pro_header[i].p_memsz > Pro_header[i].p_filesz) { /* zero padding */
                // memset((uint8_t *)ph[i].p_paddr + ph[i].p_filesz, 0, ph[i].p_memsz - ph[i].p_filesz);
            }
        }
    }

    res = f_close(fp);
    sz = PGROUNDUP(sz);

    //程序的页数必须为偶数
    if (sz % (2*PGSIZE)) sz += PGSIZE;

    //再分配两页 一页作为隔离(无法访问) 一页做栈
    if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
        goto bad;
    clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
    sp = sz;  //用户栈顶

    // Push argument strings, prepare rest of stack in ustack.
    int argc__;
    for(argc__ = 0; argc__ < argc; argc__++) {
        if(argc__ >= MAXARG)
            goto bad;

        sp = (sp - (strlen(argv[argc__]) + 1)) & ~3;

        
        if(copyout(pgdir, sp, argv[argc__], strlen(argv[argc__]) + 1) < 0)
            goto bad;

        // printf("[exec..] argv copy addr: %x\n", sp);
        ustack[3+argc__] = sp;
    }
    ustack[3+argc__] = 0;

    ustack[0] = 0xffffffff;  // fake return PC
    ustack[1] = argc;
    ustack[2] = sp - (argc+1)*4;  // argv pointer

    sp -= (3+argc+1) * 4;
    if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
        goto bad;

    //设置参数
    curproc->tf->regs[4] = argc; //a0
    curproc->tf->regs[5] = ustack[2];
    // printf("argc: %d   argv: %x\n", argc, ustack[2]);

    // Save program name for debugging.
    for(last=s=path; *s; s++)
    if(*s == '/')
        last = s+1;

    
    safestrcpy(curproc->name, last, sizeof(curproc->name));

    // Commit
    oldpgdir = curproc->pgdir;
    curproc->pgdir = pgdir;

    extern int nextasid;
    curproc->asid = nextasid++;

    curproc->sz = sz;

    curproc->tf->regs[29] = sp; //sp

    curproc->tf->cp0_status = 0x00007c00;

    curproc->tf->regs[31] = Elf_header->e_entry; //ra
    curproc->tf->pc = Elf_header->e_entry;
    curproc->tf->cp0_epc = Elf_header->e_entry;
    //返回地址放在v1
    curproc->tf->regs[3] = Elf_header->e_entry;
    //printf("pro entry: %x\n", Elf_header->e_entry);

    //要更新entryhi
    // curproc->tf->hi = (Elf_header->e_entry & 0xffffe000) | curproc->asid;

    switchuvm(curproc);
    freevm(oldpgdir);

    kfree(buffer);
    //printf("finish exec!\n");
    after_exec = 1;

    return 0;

    bad:
    f_close(fp);
    //printf("bad exec\n");
    if(pgdir) freevm(pgdir);
    after_exec = 0;
    return -1;
}