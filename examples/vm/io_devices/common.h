#ifndef common_h
#define common_h

void write(int val){
	__asm__ __volatile__ ("out %1, %0" : :  "dN" (10), "a" (val));
}

void write_b(char val){
	__asm__ __volatile__ ("outb %1, %0" : :  "dN" (10), "a" (val));
}

unsigned char read_b(unsigned short _port) {
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void exit(){
    __asm__ ("hlt");
}


#endif
