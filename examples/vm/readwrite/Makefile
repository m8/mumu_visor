.PHONY: all test clean

all: clean read.bin write.bin

read.bin: main.o
	gcc -c -ffreestanding -fno-PIE -fno-lto -nostartfiles -nostdlib -o main.o -std=c99 main_read.c
	objcopy -O binary -j .text main.o read.bin
	$(clean_object)

write.bin:
	gcc -c -ffreestanding -fno-PIE -fno-lto -nostartfiles -nostdlib -o main.o -std=c99 main.c
	objcopy -O binary -j .text main.o write.bin
	$(clean_object)

clean: 
	$(RM) *.o *.bin *.elf *.img

clean_object:
	$(RM) *.o
