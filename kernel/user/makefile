# =================================

INCLUDES	  := -I../inc/

# ======== End of configuration ===

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

.PHONY: clean

all: usys.o


include ../../include.mk