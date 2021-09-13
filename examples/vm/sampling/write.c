#include "common.h"
#include "sampling.h"

int k = 5;

void main(void) {
	CREATE_SAMPLING_PORT(1);
	WRITE_SAMPLING_PORT(0xbe);	
	exit();
}


