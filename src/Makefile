CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

#Build rule for final target (emulate)
emulate: emulate.o multiply.o data_Process.o branch.o sdatatrans.o library.o
		$(CC) emulate.o multiply.o data_Process.o branch.o sdatatrans.o library.o -o emulate

emulate.o: emulate.c emulate.h multiply.h data_Process.h branch.h sdatatrans.h library.h
		$(CC) $(CFLAGS) emulate.c -c -o emulate.o

multiply.o: multiply.c multiply.h emulate.h library.h
		$(CC) $(CFLAGS) multiply.c -c -o multiply.o

data_Process.o: data_Process.c data_Process.h emulate.h library.h
		$(CC) $(CFLAGS) data_Process.c -c -o data_process.o

sdatatrans.o: sdatatrans.c sdatatrans.h emulate.h library.h
		$(CC) $(CFLAGS) sdatatrans.c -c -o sdatatrans.o

branch.o: branch.c branch.h emulate.h library.h
		$(CC) $(CFLAGS) branch.c -c -o branch.o

library.o: library.c library.h
		$(CC) $(CFLAGS) library.c -c -o library.o

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate 
	rm -f emulate.o multiply.o data_process.o sdatatrans.o branch.o library.o
