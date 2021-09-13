.PHONY: all test clean

all: main

main:
	gcc -c -ffreestanding -fno-PIE -fno-lto -nostartfiles -nostdlib -o read.o -std=c99 read.c
	gcc -c -ffreestanding -fno-PIE -fno-lto -nostartfiles -nostdlib -o write.o -std=c99 write.c
	ld -o write -T linker.ld write.o
	ld -o read -T linker.ld read.o

clean: 
	$(RM) *.o *.bin *.elf *.img

clean_object:
	$(RM) *.o
