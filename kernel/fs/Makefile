INCLUDES	  := -I../inc/


%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $*.o

.PHONY: clean
all: fs_init.o

clean:
	rm -rf *~ *.o

include ../../include.mk
