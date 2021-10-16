drivers_dir	  := kernel/drivers
boot_dir	  := kernel/boot
init_dir	  := kernel/init
lib_dir		  := lib
fs_dir		  := kernel/fs
env_dir		  := kernel/env
mm_dir		  := kernel/mm
test_dir	  := kernel/test

sysmainc       := kernel/main.c

link_script   := scse.lds

# modules		  := kernel/boot kernel/drivers kernel/fs 
modules		  := kernel/boot kernel/test kernel/drivers kernel/fs 
AOBJECTS := $(test_dir)/*.o \
			$(boot_dir)/*.o \
			# $(drivers_dir)/*.o	 		  \
			# $(fs_dir)/*.o
# objects		  := $(boot_dir)/*.o			  \
# 				 $(drivers_dir)/*.o	 		  \
# 				 $(fs_dir)/*.o


#.PHONY是一个伪目标，可以防止在Makefile中定义的只执行命令的目标和工作
#目录下的实际文件出现名字冲突，另一种是提交执行makefile时的效率。
.PHONY: all $(modules) clean run 
ASOURCES= \

CSOURCES= \

all: $(modules) FPGA_RAM


# COBJECTS=$(CSOURCES:.c=.o)
LDFLAGS__ = -EL -nostartfiles -N -T scse.lds -O0 -G0

FPGA_RAM : $(modules) 
	$(LD)  $(LDFLAGS__) $(AOBJECTS) $(COBJECTS) -o FPGA_Ram.elf
	$(SZ) FPGA_Ram.elf
	$(OD) -D -S -l FPGA_Ram.elf > FPGA_Ram_dasm.txt
	$(OD) -D -z FPGA_Ram.elf > FPGA_Ram_modelsim.txt
	$(OC) FPGA_Ram.elf -O srec FPGA_Ram.rec

$(modules): 
	$(MAKE) --directory=$@

	
# .c.o:
# 	$(CC) $(CFLAGS) $< -o $@
	
# .S.o:
# 	$(CC) $(CFLAGS) $< -o $@



clean: 
	for d in $(modules);	\
		do					\
			$(MAKE) --directory=$$d clean; \
		done; \
	rm -rf *.o *~ $(vmlinux_elf)  $(user_disk)


include include.mk