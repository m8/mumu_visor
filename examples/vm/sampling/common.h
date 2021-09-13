#ifndef common_h
#define common_h

void write(int val){
	__asm__ __volatile__ ("out %1, %0" : :  "dN" (10), "a" (val));
}

void write_b(char val){
	__asm__ __volatile__ ("outb %1, %0" : :  "dN" (10), "a" (val));
}

static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

void exit(){
    __asm__ ("hlt");
}

#endif
