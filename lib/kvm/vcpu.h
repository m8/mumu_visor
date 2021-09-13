#ifndef VCPU_H
#define VCPU_H


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

#include "vm.h"
#include "kvm.h"
#include "defines.h"

using namespace std;


class VCPU{
    public:
        VCPU(){};
        VCPU(int, int, struct kvm_run*,int);
        int init();
        int reset();
        void* kvm_cpu_thread(void *args);
        void run(int);
        void set_ms(int);

        pthread_t vcpu_thread = 0;
        void* (*vcpu_thread_func)(void *);
        
        uint16_t vcpu_fd = 0;
        int32_t kvm_run_mmap_size = 0;
        struct kvm_run *kvm_run = nullptr;
        kvm_regs regs;   
        kvm_sregs sregs; 
        int physical_core = 0;
        int virtual_core = 0;

        sem_t mtx;
        bool lock = false;
        uint64_t ms = 2000;
        int id = 0;

    private:
        
};

#include "vcpu.cpp"

#endif