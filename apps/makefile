# =================================

INCLUDES	  := -I../inc/

# ======== End of configuration ===

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: console.o bash.o help.o


include ../include.mk