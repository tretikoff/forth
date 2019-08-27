all: forth

forth: forth.o
	ld -o forth forth.o

forth.o: forth.asm
	nasm -f elf64 -g -F dwarf forth.asm -o forth.o

clean:
	rm -f forth *.o

# debugging: gdb forth -f .gdbinit
=======
rebuild: clean forth

debug: forth
	gdb forth -f .gdbinit

# build and debug
bd: rebuild debug
