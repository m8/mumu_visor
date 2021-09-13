int k = 5;

void main(void) {
    volatile int i;	
	unsigned short port = 0x10;
	
	
    for (i = 0; i < 4; i++) { 	
    	__asm__ __volatile__ ("out %1, %0" : :  "dN" (10), "a" (k));
	}
	

    __asm__ ("hlt");
}

