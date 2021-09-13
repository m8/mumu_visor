.PHONY: all test clean

all: main

main:
	gcc -c -ffreestanding -fno-PIE -fno-lto -nostartfiles -nostdlib -o main.o -std=c99 io.c
	ld -o prog -T linker.ld main.o

clean: 
	$(RM) *.o *.bin *.elf *.img

clean_object:
	$(RM) *.o
