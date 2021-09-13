#pragma once

#include <stdint.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <semaphore.h>
#include <chrono>
#include <linux/kvm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <thread>

#include "vcpu.h"
#include "defines.h"
#include "kvm.h"
#include "../hw/serial.hpp"

VCPU::VCPU(int fd, int mmap_size, struct kvm_run *_kvm_run, int _id)
{
    this->vcpu_fd = fd;
    this->kvm_run_mmap_size = mmap_size;
    this->id = _id;
    this->kvm_run = _kvm_run;
}

void VCPU::run(int cpu, int sec = 0)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    sched_setaffinity(this->vcpu_thread, sizeof(cpuset), &cpuset);
    sem_init(&this->mtx, 0, 0);
    pthread_create(&(this->vcpu_thread), NULL, vcpu_thread_func, this);
    cout << "core: " << cpu << " " << this->vcpu_thread << endl;
    
    if(sec != 0){
        sleep(sec);
        pthread_cancel(this->vcpu_thread);
    }
}

int VCPU::run_real_mode()
{
    // Get segment registers from VCPU 
    if (ioctl(this->vcpu_fd, KVM_GET_SREGS, &this->sregs) < 0) {
		perror("KVM_GET_SREGS");
		exit(1);
	}

    // Do not add modify any segment registers in real mode
    // In real mode use code segment directly
    this->sregs.cs.selector = 0x0;
    this->sregs.cs.base     = 0x0;

    // Set segment registers
    if (ioctl(this->vcpu_fd, KVM_SET_SREGS, &this->sregs) < 0)
    {
        perror("ERROR: KVM_GET_SREGS");
        return -1;
    }

    // Set instruction pointer to 0x0 rflags should always 2
    this->regs.rflags = 0x2; 
	this->regs.rip = 0x0;

	if (ioctl(this->vcpu_fd, KVM_SET_REGS, &regs) < 0) {
		perror("KVM_SET_REGS");
		exit(1);
	}

    return 0;
}

void VCPU::setup_segment_registers(){
    
    struct kvm_segment seg = {
        .base = 0,
        .limit = 0xffffffff,
        .selector = 1 << 3,
        .type = 11, 
        .present = 1,
        .dpl = 0,
        .db = 1,
        .s = 1,
        .l = 0,
        .g = 1,
    };
    
    seg.type = 3;
    seg.selector = 2 << 3;
    
    this->sregs.cr0 |= 1u;
    this->sregs.cs = seg;
    this->sregs.ds = this->sregs.es = this->sregs.fs = this->sregs.gs = this->sregs.ss = seg;
}


char consume_buffer(string & buffer){
    if(buffer.length() == 0){
        return '\0';
    } else {
        char b = buffer[0];
        buffer.erase(0,1);
        return b;
    }
}

// ~ VCPU Thread ~
// KVM_RUN returns
//
void *VCPU::kvm_cpu_thread(void *args)
{
    uint16_t ret = 0;
    bool vm_exit = false;

    // this->run_real_mode();
    cout << "CPU init with id: " << gettid() << " core: " << this->physical_core << endl;
#ifdef SCHED_ON
lock:
    sem_wait(&this->mtx);
#endif

    while (!vm_exit) {
        // printf("id: %d core: %d \n", gettid(), this->physical_core, this->id);

        ret = ioctl(this->vcpu_fd, KVM_RUN, NULL);

#ifdef SCHED_ON
        if (this->lock == 1) {
            goto lock;
        }
#endif
        if (ret < 0) {
            fprintf(stderr, "KVM_RUN failed\n");
            exit(1);
        }
        switch (this->kvm_run->exit_reason) {
            case KVM_EXIT_IO: {
                if(this->kvm_run->io.direction == 0 && this->kvm_run->io.port == 0x2f8){
                    *(int *)((char *)(this->kvm_run) + this->kvm_run->io.data_offset) = consume_buffer(this->buffer);
                }
                else{
                    // port_write(*(int *)((char *)(this->kvm_run) + this->kvm_run->io.data_offset));
                    printf("id: %d core: %d id: %d data: %c\n", gettid(), this->physical_core, this->id, *(int *)((char *)(this->kvm_run) + this->kvm_run->io.data_offset));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                break;
            }
            case KVM_EXIT_MMIO: {
                
                cout << "HEALTH MONITOR: (mmio) core: "<< this->physical_core << " vm id: "<< this->id  << endl;
                break;
            }
            case KVM_EXIT_HLT: {
                cout << "halted" << endl;
                vm_exit = true;
                break;
            }
            default: {
                printf("KVM ERROR\n %d", this->kvm_run->exit_reason);
                sleep(1);
                vm_exit = false;
            }
        }
    }
    return NULL;
}


// Set physical and virtual cores of VCPU
void VCPU::set_cores(int _virtual_core, int _physical_core)
{
    this->virtual_core = _virtual_core;
    this->physical_core = _physical_core;
}

