#include "common.h"
#include "sampling.h"

void main(void) {
	
	//Wait 5 second
    for (int k = 0; k < 2; k++) { 	
		write(0);
	}
	CREATE_SAMPLING_PORT(2);
	int val = READ_SAMPLING_PORT();
	// WRITE_SAMPLING_PORT(12);
	write(val);
	exit();
}


