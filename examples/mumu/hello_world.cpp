#include <iostream>
#include <list>
#include <fstream>

#include "../../src/hypervisor/kvm/kvm.h"
#include "../../src/hypervisor/kvm/vm.h"
#include "../../src/hypervisor/kvm/vcpu.h"
#include "../../src/hypervisor/kvm/modes.h"

#include "../../src/hypervisor/loader/mumu_elf.hpp"
#include <inttypes.h>

using namespace std;

int main()
{
    KVM *_kvm = new KVM();
    _kvm->init();

    VM *vm = new VM(_kvm);
    vm->create_vcpu(0, 0, 0);

    vm->mem_init();

    load_elf("/home/musa/Desktop/Tübitak/mumu_visor/examples/vm/hello_world/prog",vm);
    auto s_address = get_entry_address("/home/musa/Desktop/Tübitak/mumu_visor/examples/vm/hello_world/prog");
    
    run_long_mode(vm,vm->vcpus[0],s_address);

    vm->run_vm();
    vm->stop_vm();
}