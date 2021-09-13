#pragma once

#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#include "vm.h"
#include "kvm.h"
#include "defines.h"

// #define SCHED_ON

using namespace std;

class VCPU {
public:
    // Constructors
    VCPU(){};

    VCPU(int, int, struct kvm_run*,int);

    // Methods
    int init();

    // VCPU running modes
    int run_real_mode();
    int run_protected_mode();


    void* kvm_cpu_thread(void *args);

    void run(int,int);

    void set_ms(int);

    void set_cores(int, int);

    void page_tables(void* mem, struct kvm_sregs *sregs);
    
    void setup_segment_registers();

    void* (*vcpu_thread_func)(void *);    

    // Member Variables
    sem_t mtx;
    pthread_t vcpu_thread = 0;
    
    bool lock = false; 
    int physical_core = 0;
    int virtual_core = 0;
    int id = 0; 
    
    kvm_sregs sregs; 
    kvm_regs regs;   

    uint16_t vcpu_fd = 0;

    string buffer = "";

private:

    struct kvm_run *kvm_run = nullptr;
    int32_t kvm_run_mmap_size = 0;

};

#include "vcpu.cpp"

