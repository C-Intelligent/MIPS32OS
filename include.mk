# microAptiv_UP makefile for MIPSfpga

ifndef MIPS_ELF_ROOT
$(error MIPS_ELF_ROOT must be set to point to toolkit installation root)
endif

CC=mips-mti-elf-gcc
LD=mips-mti-elf-gcc
OD=mips-mti-elf-objdump
OC=mips-mti-elf-objcopy
SZ=mips-mti-elf-size

CFLAGS = -O1 -g -EL -c -msoft-float -march=m14kc -G0 #-mcmodel=medium
LDFLAGS = -EL -msoft-float -march=m14kc -Wl,-Map=FPGA_Ram_map.txt

# Set up the link addresses for a bootable C program on MIPSfpga
LDFLAGS += -T uhi32.ld
# Place the boot code (physical address). The virtual address for
# boot code entry point is hard-wired to 0x9fc00000.
LDFLAGS += -Wl,--defsym,__flash_start=0xbfc00000
# Place the application code (physical address)
LDFLAGS += -Wl,--defsym,__flash_app_start=0x80000000
# Place the application code (virtual address)
LDFLAGS += -Wl,--defsym,__app_start=0x80000000
# Set the stack to the top of the Code/Data RAM
LDFLAGS += -Wl,--defsym,__stack=0x80040000
# Cautiously set the size of memory as the 2015.01 toolchain uses
# this size as the amount of free memory between the end of the
# program data and the lowest address that the stack will reach.
#
# Max 2K for stack (0x800)
# Max 128K for program code/data (0x20000)
# Leaving 126K heap (0x1f800)
LDFLAGS += -Wl,--defsym,__memory_size=0x1f800
# Set the entry point to the true hard-reset address
LDFLAGS += -Wl,-e,0xbfc00000
