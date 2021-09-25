#ifndef VM_H
#define VM_H

#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <linux/kvm.h>
#include <vector>

#include "vcpu.hpp"
#include "kvm.hpp"

class VM
{
public:
    VM(KVM *_ref);

    VCPU* create_vcpu(int, uint8_t, uint8_t);

    void load_binary(const string _file);
    void load_binary_data(const string _file);

    void load_text_binary(uint8_t[],int);
    
    void run_vm(int);
    void stop_vm();

    int vm_irq_line(int, int);
    int vm_irq_trigger(int);

    int mem_init(unsigned long long);


    // Public variables
    VCPU* vcpus [4] = {};
    VCPU *vcpu = nullptr;
    KVM* kvm_ref;
    uint32_t id = 0;
    uint16_t fd = 0;
    uint64_t top_address = RAM_SIZE;
    uint64_t entry_address = 0x0;
    char* mem;

private:
  
};


#include "vm.cpp"

#endif