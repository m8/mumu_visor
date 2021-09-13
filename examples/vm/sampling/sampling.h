#ifndef sampling_h
#define sampling_h

#define ADDRESS_BASE 0x140000

#include "common.h"

struct SAMPLING_PORT{
    int port1;
    int port2;
    int name;
    int flip;
	long long cc;
};

struct CHANNELS{
    int names[10];
    int addresses[10];
    int size;
};

struct SAMPLING_PORT* PORT = 0x0;

void CREATE_SAMPLING_PORT(int name){
	// Should find under array with for loop
	int add = 0;
	struct CHANNELS* channel = (struct CHANNELS*)(ADDRESS_BASE);
	for(int k = 0; k < channel->size; k++){
		if(channel->names[k] == name){
			PORT = (struct SAMPLING_PORT*)(channel->addresses[add]);
			return;
		}
	}
	write(-1);
};

int WRITE_SAMPLING_PORT(int val){
	if(PORT == 0x0){
		write(1);
		return -1;	
	}
	if(PORT->flip == 0){
		PORT->port1 = val;
		PORT->flip = 1;
	} else {
		PORT->port2 = val;
		PORT->flip = 0;
	}
};


int READ_SAMPLING_PORT(){
	if(PORT == 0x0){
		write(1);
		return -1;	
	}
	if(PORT->flip == 1){
		return PORT->port1;
	} else {
		return PORT->port2;
	}
};

#endif
