all: itc

itc: itc.o
	ld -o itc itc.o

itc.o: itc.asm
	nasm -f elf64 -g -F dwarf itc.asm -o itc.o

clean:
	rm -f itc *.o

# debugging: gdb itc -tui
