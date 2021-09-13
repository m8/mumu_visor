#include "common.h"
#include "serial.h"

int k = 5;

void main(void) {

	for(int k = 0; k < 11; k++){
		int a = read_b(COM_BASE);
		write(a);
	}

	exit();
}


