int k = 5;
void main(void) {
    volatile int i;
	long long array [50][40] = {0};
	array[1][1] = 'm';

	
	unsigned short port = 0x10;

	// *(long *) 0x300000 = 42;
	int* asd = (int *) 0x600000;

    for (i = 0; i < 4; i++) { 	
    	__asm__ __volatile__ ("out %1, %0" : :  "dN" (10), "a" (*asd));
	}
	

    __asm__ ("hlt");
}
