#ifndef VM_H
#define VM_H

#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <linux/kvm.h>
#include <vector>

#include "vcpu.h"
#include "kvm.h"

class VM
{
public:
    VM(KVM *_ref);
    void load_binary(const string _file);
    VCPU* create_vcpu(int, uint8_t, uint8_t);
    int mem_init();

    void run_vm();
    void stop_vm();


    int id = 0;
    uint16_t fd = 0;
    uint64_t mem = 0;
    VCPU* vcpus [4] = {};
    VCPU *vcpu = nullptr;
    KVM* kvm_ref;

private:
  
};


#include "vm.cpp"

#endif