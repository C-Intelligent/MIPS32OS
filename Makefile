drivers_dir	  := kernel/drivers
boot_dir	  := kernel/boot
init_dir	  := kernel/init
lib_dir		  := kernel/lib
fs_dir		  := kernel/fs
proc_dir	  := kernel/proc
mem_dir		  := kernel/mem
fdc_dir       := kernel/libfdc
trap_dir      := kernel/trap
user_dir      := kernel/user

link_script   := scse.lds

# modules		  := kernel/boot kernel/drivers kernel/fs 
modules		  := kernel/boot kernel/drivers \
 					kernel/fs kernel/init kernel/mem kernel/lib \
					 kernel/trap kernel/fs  kernel/user\
					kernel/proc apps\
					# kernel/apps

AOBJECTS := $(boot_dir)/*.o \
			$(drivers_dir)/*.o	 		  \
			$(mem_dir)/*.o \
			$(lib_dir)/*.o \
			$(trap_dir)/*.o \
			$(fs_dir)/*.o \
			$(init_dir)/*.o \
			$(proc_dir)/*.o \
			# $(fdc_dir)/*.o   #$(user_dir)/*.o \

LIBS := 	$(user_dir)/*.o	 		  \
			# $(proc_dir)/*.o \
			

APPS := console bash help
			


#.PHONY是一个伪目标，可以防止在Makefile中定义的只执行命令的目标和工作
#目录下的实际文件出现名字冲突，另一种是提交执行makefile时的效率。
.PHONY: all $(modules) clean run 
ASOURCES= \

CSOURCES= \

all: $(modules) FPGA_RAM $(APPS)


# COBJECTS=$(CSOURCES:.c=.o)
LDFLAGS__ = -EL -nostartfiles -N -T scse.lds -O0 -G0 

FPGA_RAM : $(modules) 
	$(LD)  $(LDFLAGS__) $(AOBJECTS)  -o vmlinux
	$(SZ) vmlinux
	$(OD) -D -S -l vmlinux > vmlinux_dasm.txt
	$(OD) -D -z vmlinux > vmlinux_modelsim.txt
	$(OC) vmlinux -O srec vmlinux.rec

APPFLAGS = -EL -nostartfiles -N -T user.lds -O0 -G0
$(APPS):
	$(LD) $(APPFLAGS) $(LIBS) apps/$@.o -o apps/$@
	$(SZ) apps/$@
	$(OD) -D -S -l apps/$@ > apps/$@_dasm.txt
	$(OD) -D -z apps/$@ > apps/$@_modelsim.txt

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