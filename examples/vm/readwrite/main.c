void main(void) {
    volatile int i;	
	unsigned short port = 0x10;
	
	int* c1 = (int*) 0x8000;
	long channel_address = *c1; 
	
	//int* channel_address = (int *) 0x400000;
	int* shared = (int *)channel_address;
	*shared = 0xbeab;

    for (i = 0; i < 4; i++) { 	
    	__asm__ __volatile__ ("out %1, %0" : :  "dN" (10), "a" ('a'));
	}
	

    __asm__ ("hlt");
}

