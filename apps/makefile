# =================================

INCLUDES	  := -I../inc/

# ======== End of configuration ===

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: console.o bash.o help.o demo.o ls.o echo.o cat.o grep.o ptest.o ln.o rm.o mkdir.o sitop.o


include ../include.mk