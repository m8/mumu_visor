.PHONY: all test clean

all: clean test.bin

test.bin: main.o
	ld -m elf_i386 --oformat binary -N -e main -o main.bin main.o

main.o:
	gcc -c -m16 -ffreestanding -fno-PIE -fno-lto -nostartfiles -nostdlib -o main.o -std=c99 main.c

clean: 
	$(RM) *.o *.bin *.elf *.img
