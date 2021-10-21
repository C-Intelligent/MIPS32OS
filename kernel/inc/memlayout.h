#include "types.h"

#define KERNEL_BASE 0x80000000
#define KERNEL_TEXT_START 0x80001000
#define KERNEL_ELF_END 0x80400000
#define MAPPED_RAM_START KERNEL_ELF_END   //虚拟地址映射起始位置 (这是一个虚拟地址)
#define PHYS_PG_SIZE 4*1024    //页框大小
#define PHYS_PG_NUM 1024  //物理页框数量
#define MAPPED_RAM_STOP (MAPPED_RAM_START + PHYS_PG_NUM * PHYS_PG_SIZE)    //虚拟地址映射起始位置

#define ADDR2PTR(a) ((void *) (a))

