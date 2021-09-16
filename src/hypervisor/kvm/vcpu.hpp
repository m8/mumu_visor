#pragma once

#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#include "vm.hpp"
#include "kvm.hpp"
#include "defines.h"

// #define SCHED_ON

using namespace std;

class VCPU {
public:
    // Constructors
    VCPU() = default;

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

    void set_page_tables(void* mem, struct kvm_sregs *sregs);
    
    void set_segment_registers();

    void* (*vcpu_thread_func)(void *);    

    // Member Variables
    int id, vcpu_fd, physical_core, virtual_core = 0;
    bool lock = false; 
  
    sem_t mtx;
    pthread_t vcpu_thread = 0;
    
    kvm_sregs sregs; 
    kvm_regs regs;   

    string buffer = "";

private:

    struct kvm_run *kvm_run = nullptr;
    int32_t kvm_run_mmap_size = 0;

};

#include "vcpu.cpp"

