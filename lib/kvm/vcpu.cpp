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

#include "vcpu.h"
#include "defines.h"
#include "kvm.h"

// extern sem_t consuming_sem;


VCPU::VCPU(int fd, int mmap_size, struct kvm_run* _kvm_run, int _id){
    this->vcpu_fd = fd;
    this->kvm_run_mmap_size = mmap_size;
    this->id = _id;
    this->kvm_run = _kvm_run;
}

void VCPU::run(int cpu){
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);     
    CPU_SET(cpu, &cpuset); 
    sched_setaffinity(this->vcpu_thread, sizeof(cpuset), &cpuset);
    sem_init(&this->mtx, 0, 0);
    pthread_create(&(this->vcpu_thread), NULL, vcpu_thread_func, this);
    cout <<"core: "  << cpu <<  " " << this->vcpu_thread << endl;
}


int VCPU::reset(){
    
    if (ioctl(this->vcpu_fd, KVM_GET_SREGS, &(this->sregs)) < 0)
    {
        perror("ERROR: KVM_GET_SREGS");
        return -1;
    }

    this->sregs.cs.selector = CODE_START;
    this->sregs.cs.base = CODE_START * 16;
    this->sregs.ss.selector = CODE_START;
    this->sregs.ss.base = CODE_START * 16;
    this->sregs.ds.selector = CODE_START;
    this->sregs.ds.base = CODE_START * 16;
    this->sregs.es.selector = CODE_START;
    this->sregs.es.base = CODE_START * 16;
    this->sregs.fs.selector = CODE_START;
    this->sregs.fs.base = CODE_START * 16;
    this->sregs.gs.selector = CODE_START;

    if (ioctl(this->vcpu_fd, KVM_SET_SREGS, &this->sregs) < 0)
    {
        perror("ERROR: KVM_GET_SREGS");
        return -1;
    }

    this->regs.rflags = 0x0000000000000002ULL;
    this->regs.rip = 0;
    this->regs.rsp = 0xffffffff;
    this->regs.rbp = 0;

    if (ioctl(this->vcpu_fd, KVM_SET_REGS, &(this->regs)) < 0)
    {
        perror("EROR: KVM_SET_REGS\n");
        return -1;
    }
    return 0;
}




// ~ VCPU Thread ~ 
// KVM_RUN returns 
//
void* VCPU::kvm_cpu_thread(void *args)
{    
    uint16_t ret = 0;
    bool vm_exit = false;

    reset();

lock:
    sem_wait(&this->mtx);
    while (!vm_exit)
    {
        ret = ioctl(this->vcpu_fd, KVM_RUN, NULL);

        if(this->lock == 1){
            goto lock;
        }

        if (ret < 0)
        {
            fprintf(stderr, "KVM_RUN failed\n");
            exit(1);
        }

        switch (this->kvm_run->exit_reason)
        {
        case KVM_EXIT_IO:
        {
            printf("core: %d id: %d data: %d\n", this->physical_core, this->id, *(int *)((char *)(this->kvm_run) + this->kvm_run->io.data_offset));
            sleep(1);
            break;
        }
        case KVM_EXIT_HLT:
        {
            vm_exit = true;
            break;
        }
        default:
        {
            printf("KVM ERROR\n %d", this->kvm_run->exit_reason);
            vm_exit = false;
        }
        }
    }
    return NULL;
}

void VCPU::set_ms(int _ms){
    this->ms = _ms;
}