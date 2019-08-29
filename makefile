all: forth

forth: forth.o
	ld -o forth forth.o

forth.o: forth.asm
	nasm -f elf64 -g -F dwarf forth.asm -o forth.o

clean:
	rm -f forth *.o

# debugging: gdb forth -f .gdbinit
rebuild: clean forth

debug: forth
	gdb forth -f .gdbinit

run: forth
	./forth

test1: clean forth
	./forth <tests/test01.frt

test2: clean forth
	./forth <tests/test02.frt

test3: clean forth
	./forth <tests/test03.frt

test4: clean forth
	./forth <tests/test04.frt

test5: clean forth
	./forth <tests/test05.frt

test6: clean forth
	./forth <tests/test06.frt

test7: clean forth
	./forth <tests/test07.frt

test8: clean forth
	./forth <tests/test08.frt

stdlib: clean forth
	./forth <libs/stdlib.frt

mp: clean forth
	./forth <mp.frt

# build and debug
bd: clean debug

# build and run
br: clean run
