#include <iostream>
#include <list>
#include <fstream>

#include "../src/hypervisor/kvm/kvm.h"
#include "../src/hypervisor/kvm/vm.h"
#include "../src/hypervisor/kvm/vcpu.h"
#include "../src/hypervisor/kvm/modes.h"

using namespace std;


int main()
{
    KVM *_kvm = new KVM();
    _kvm->init();

    
    VM *vm = new VM(_kvm);
    vm->create_vcpu(0,0,0);

    vm->mem_init();
    
    vm->load_binary("/home/musa/Desktop/Tübitak/mumu_visor/Hello World/c_hello_world/main.bin");
    setup_protected_mode(vm->vcpus[0]);
    cout << "brad" << endl;
    vm->run_vm();
    vm->stop_vm();
}