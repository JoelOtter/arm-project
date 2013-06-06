CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

#Build rule for final target (emulate)
emulate: emulate.o multiply.o data_Process.o branch.o sdatatrans.o library.o
		$(CC) $^ -o $@

emulate.o: emulate.c emulate.h multiply.h data_Process.h branch.h sdatatrans.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

multiply.o: multiply.c multiply.h emulate.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

data_Process.o: data_Process.c data_Process.h emulate.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

sdatatrans.o: sdatatrans.c sdatatrans.h emulate.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

branch.o: branch.c branch.h emulate.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

library.o: library.c library.h
		$(CC) $(CFLAGS) $< -c -o $@

assemble: assemble.o symboltableadt.o ass_multiply.o
		$(CC) $^ -o $@

assemble.o: assemble.c assemble.h symboltableadt.h ass_multiply.h
		$(CC) $(CFLAGS) $< -c -o $@

symboltableadt.o: symboltableadt.c symboltableadt.h
		$(CC) $(CFLAGS) $< -c -o $@

ass_multiply.o: ass_multiply.c ass_multiply.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate


clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate 
	rm -f emulate.o multiply.o data_process.o sdatatrans.o branch.o library.o
	rm -f assemble.o symboltableadt.o ass_multiply.o library.o


