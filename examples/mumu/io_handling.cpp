#include <iostream>
#include <list>
#include <fstream>

#include "../src/hypervisor/kvm/kvm.h"
#include "../src/hypervisor/kvm/vm.h"
#include "../src/hypervisor/kvm/vcpu.h"
#include "../src/hypervisor/kvm/modes.h"

#include "../src/hypervisor/loader/mumu_elf.hpp"
#include "../src/hypervisor/hw/serial.hpp"
#include <inttypes.h>

#include <fstream>

using namespace std;

int main()
{
    KVM *_kvm = new KVM();
    _kvm->init();

    VM *vm = new VM(_kvm);
    vm->create_vcpu(0, 0, 0);
    vm->mem_init();

    // open_linux_serial();

    // for(int i = 0; i <10; i++){
    //     port_write('i');
    // }
    

    // return 0;
    // for (int k = 0;vm->vcpus[0]->buffer.length(); k++ ){
    //     cout << "==> " << consume_buffer(vm->vcpus[0]->buffer) << endl;
    // }

    load_elf("/home/musa/Desktop/Tübitak/mumu_visor/Hello World/IO Devices/prog",vm);
    auto s_address = get_entry_address("/home/musa/Desktop/Tübitak/mumu_visor/Hello World/IO Devices/prog");
    
    // open_linux_serial();
    // auto k = porkt_read();
    vm->vcpus[0]->buffer = "Hello World";

    run_long_mode(vm,vm->vcpus[0],s_address);
    // // setup_protected_mode(vm->vcpus[0]);
    // cout << "brad" << endl;
    // pthread_t console;
    // pthread_create(&console, NULL, port_read, NULL);

    vm->run_vm();
    vm->stop_vm();
}