.PHONY: all test clean

all: main

main: main.c
	gcc -c -ffreestanding -fno-PIE -fno-lto -nostartfiles -nostdlib -o main.o -std=c99 main.c
	ld -o prog -T linker.ld main.o
	objcopy -O binary -j .text prog text.bin
	objcopy -O binary -j .data prog data.bin

clean: 
	$(RM) *.o *.bin *.elf *.img

clean_object:
	$(RM) *.o
