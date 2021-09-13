#include <iostream>
#include <list>
#include <fstream>

#include "../src/hypervisor/kvm/kvm.h"
#include "../src/hypervisor/kvm/vm.h"
#include "../src/hypervisor/kvm/vcpu.h"
#include "../src/hypervisor/kvm/modes.h"

#include "../src/hypervisor/loader/elf.hpp"
#include "../src/hypervisor/loader/mumu_elf.hpp"

#include <inttypes.h>

using namespace std;
vector<elf_parser::section_t> secs;

int main()
{
    KVM *_kvm = new KVM();
    _kvm->init();

    VM *vm = new VM(_kvm);
    vm->create_vcpu(0, 0, 0);

    vm->mem_init();

    elf_parser::Elf_parser parser("/home/musa/Desktop/Tübitak/mumu_visor/Hello World/elf/prog");
    long n = get_entry_address("/home/musa/Desktop/Tübitak/mumu_visor/Hello World/elf/prog");
    
    
    vector<elf_parser::segment_t> segs = parser.get_segments();
    auto sym = parser.get_symbols();
    secs = parser.get_sections();    

    auto map = parser.get_memory_map();

    char *text = (char *)vm->mem;

    for(auto sec: secs){
        memcpy(text + sec.section_offset,map + sec.section_offset, sec.section_size );
    }

    //memcpy(text + get_section_from_name(".text").section_addr,map + get_section_from_name(".text").section_offset, get_section_from_name(".text").section_size );
    //memcpy(text + get_section_from_name(".data").section_addr,map + get_section_from_name(".data").section_offset, get_section_from_name(".data").section_size );
    
    run_long_mode(vm,vm->vcpus[0],n);
    // // setup_protected_mode(vm->vcpus[0]);
    // cout << "brad" << endl;
    vm->run_vm();
    vm->stop_vm();
}