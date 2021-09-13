#ifndef common_h
#define common_h

void write(int val){
	__asm__ __volatile__ ("out %1, %0" : :  "dN" (10), "a" (val));
}

void write_b(char val){
	__asm__ __volatile__ ("outb %1, %0" : :  "dN" (10), "a" (val));
}

void exit(){
    __asm__ ("hlt");
}

#endif
