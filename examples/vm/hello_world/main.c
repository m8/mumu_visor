#include "common.h"

void main(void) {
	
	char hello_world[12] = {'H','e','l','l','o',' ','W','o','r','l','d'};

    for (int k = 0; k < 12; k++) { 	
		write(hello_world[k]);
	}
	
	exit();
}


