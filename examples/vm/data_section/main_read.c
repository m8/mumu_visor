int k = 5;
void main(void) {
    volatile int i;
	long long array [50][40] = {0};
	array[1][1] = 'm';

	// *(long *) 0x300000 = 42;
	int* c1 = (int*) 0x8000;
	long address = *c1; 
	//long address = 0x400000;
	int* shared = (int *)address;

    for (i = 0; i < 4; i++) { 	
    	__asm__ __volatile__ ("out %1, %0" : :  "dN" (10), "a" (*shared));
	}
	

    __asm__ ("hlt");
}
