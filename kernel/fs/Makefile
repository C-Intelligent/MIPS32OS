INCLUDES	  := -I../inc/


%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $*.o

.PHONY: clean
all: file.o sysfile.o pipe.o

clean:
	rm -rf *~ *.o

include ../../include.mk
