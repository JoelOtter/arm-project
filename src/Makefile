CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

#Build rule for final target (emulate)

all: assemble emulate

emulate: emulate.o emu_multiply.o emu_data_process.o emu_branch.o emu_data_transfer.o library.o
		$(CC) $^ -o $@

emulate.o: emulate.c emulate.h emu_multiply.h emu_data_process.h emu_branch.h emu_data_transfer.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

emu_multiply.o: emu_multiply.c emu_multiply.h emulate.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

emu_data_process.o: emu_data_process.c emu_data_process.h emulate.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

emu_data_transfer.o: emu_data_transfer.c emu_data_transfer.h emulate.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

emu_branch.o: emu_branch.c emu_branch.h emulate.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

library.o: library.c library.h
		$(CC) $(CFLAGS) $< -c -o $@

assemble: assemble.o ass_symbol_table.o ass_multiply.o ass_branch.o ass_special.o ass_data_transfer.o library.o ass_data_processing.o
		$(CC) $^ -o $@

assemble.o: assemble.c assemble.h ass_symbol_table.h ass_multiply.h ass_special.h ass_branch.h ass_data_transfer.h ass_data_processing.h
		$(CC) $(CFLAGS) $< -c -o $@

ass_symbol_table.o: ass_symbol_table.c ass_symbol_table.h
		$(CC) $(CFLAGS) $< -c -o $@

ass_multiply.o: ass_multiply.c ass_multiply.h library.h
		$(CC) $(CFLAGS) $< -c -o $@
		
ass_branch.o: ass_branch.c ass_branch.h assemble.h library.h ass_symbol_table.h
		$(CC) $(CFLAGS) $< -c -o $@
		
ass_special.o: ass_special.c ass_special.h library.h
		$(CC) $(CFLAGS) $< -c -o $@

ass_data_transfer.o: ass_data_transfer.c ass_data_transfer.h library.h assemble.h ass_symbol_table.h
		$(CC) $(CFLAGS) $< -c -o $@ 

ass_data_processing.o: ass_data_processing.c ass_data_processing.h library.h
		$(CC) $(CFLAGS) $< -c -o $@   

.SUFFIXES: .c .o

.PHONY: all clean


clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate 
	rm -f emulate.o emu_multiply.o emu_data_process.o emu_data_transfer.o emu_branch.o library.o
	rm -f assemble.o ass_symbol_table.o ass_multiply.o ass_branch.o ass_special.o library.o ass_data_transfer.o


