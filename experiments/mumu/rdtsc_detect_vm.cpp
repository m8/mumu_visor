#include <iostream>
#include <list>
#include <fstream>

#include "../../src/hypervisor/kvm/kvm.hpp"
#include "../../src/hypervisor/kvm/vm.hpp"
#include "../../src/hypervisor/kvm/vcpu.hpp"
#include "../../src/hypervisor/kvm/modes.h"

#include "../../src/hypervisor/loader/mumu_elf.hpp"
#include <inttypes.h>

using namespace std;

static inline unsigned long long rdtsc_diff_vmexit() {
    unsigned long long ret, ret2;
    unsigned eax, edx;
    
    __asm__ volatile("rdtsc" : "=a" (eax), "=d" (edx));
    ret  = ((unsigned long long)eax) | (((unsigned long long)edx) << 32);

    __asm__ volatile("cpuid" : : "a"(0x00));

    __asm__ volatile("rdtsc" : "=a" (eax), "=d" (edx));
    ret2  = ((unsigned long long)eax) | (((unsigned long long)edx) << 32);
    return ret2 - ret;
}

int main()
{
    KVM *_kvm = new KVM();
    _kvm->init();

    VM *vm = new VM(_kvm);
    vm->create_vcpu(0, 0, 0);

    vm->mem_init();

    load_elf("/home/musa/Desktop/Tübitak/mumu_visor/experiments/vm/rdtsc_detect_vm/prog",vm);
    auto s_address = get_entry_address("/home/musa/Desktop/Tübitak/mumu_visor/experiments/vm/rdtsc_detect_vm/prog");    
    
    run_long_mode(vm,vm->vcpus[0],s_address);

    std::cout << "------------ \nVM RES \n";
    vm->run_vm();
    vm->stop_vm();

    auto res = rdtsc_diff_vmexit();
    std::cout << "----------- \nNormal CPUID instruction: " << res << "\n"; 
}

// Result
// KVM version 12
// vcpu init success
// vm created 
// Testing 64-bit mode
// ------------ 
// VM RES 
// core: 0 140737341830912
// CPU init with id: 169864 core: 0
// id: 169864 core: 0 id: 0 data: 3726
// halted
// ----------- 
//  Normal CPUID instruction: 342