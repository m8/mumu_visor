#include <iostream>
#include <list>

#include "../src/hypervisor/kvm/kvm.h"
#include "../src/hypervisor/kvm/vm.h"
#include "../src/hypervisor/kvm/vcpu.h"

#include "../src/xml/xml_parser.h"
#include <fstream>

using namespace std;


int main()
{
    KVM *_kvm = new KVM();
    _kvm->init();

    
    VM *vm = new VM(_kvm);
    vm->create_vcpu(0,0,0);
           
    vm->mem_init();
    vm->load_binary("/home/musa/Desktop/Tübitak/kvmsample-master/test.bin");
    
    vm->run_vm();
    vm->stop_vm();

}