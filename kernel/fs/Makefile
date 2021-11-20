INCLUDES	  := -I../inc/


%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $*.o

.PHONY: clean
all: fs_init.o file.o sysfile.o

clean:
	rm -rf *~ *.o

include ../../include.mk
