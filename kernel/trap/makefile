# =================================

INCLUDES	  := -I../inc/

# ======== End of configuration ===

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: _tlb.o trap.o sys.o syscall.o timer.o handle_int.o swtch.o exec.o

# main.S: main.c
# 	$(CC) $^ -o $@
# test.elf: test.S
# 	$(CC) -EL -nostartfiles -N -T $(link_script) -O0 -G0 $^ -o $@


include ../../include.mk