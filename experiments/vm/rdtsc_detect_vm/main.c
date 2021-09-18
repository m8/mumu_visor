#include "common.h"


static inline unsigned long long rdtsc_diff_vmexit() {
    unsigned long long ret, ret2;
    unsigned eax, edx;
    __asm__ volatile("rdtsc" : "=a" (eax), "=d" (edx));
    ret  = ((unsigned long long)eax) | (((unsigned long long)edx) << 32);

    __asm__ volatile("cpuid" : : "a"(0x00));

    __asm__ volatile("rdtsc" : "=a" (eax), "=d" (edx));
    ret2  = ((unsigned long long)eax) | (((unsigned long long)edx) << 32);
    return ret2 - ret;
}


void main(void) {
	
	char hello_world[12] = {'H','e','l','l','o',' ','W','o','r','l','d'};
	
	long long k = rdtsc_diff_vmexit();
   	write(k);
	
	exit();
}


