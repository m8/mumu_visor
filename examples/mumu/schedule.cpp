#include <iostream>
#include <list>

#include "src/hypervisor/kvm/kvm.hpp"
#include "src/hypervisor/kvm/vm.hpp"
#include "src/hypervisor/kvm/vcpu.hpp"
#include "src/scheduler/sch2.hpp"

#include "src/xml/xml_parser.h"
#include <fstream>

#define SCHED_ON

using namespace std;

int main()
{
    KVM *_kvm = new KVM();
    _kvm->init();

    xml_parser* parser = new xml_parser();
    auto parsed_vms =  parser->parse("/home/musa/Desktop/Tübitak/Source/test.xml");
    
    auto vm_list = new list<VM*>();
    for(auto l: *parsed_vms){
        VM *vm = new VM(_kvm);
        for(auto k = 0; k < l->cores.size(); k++){
            vm->create_vcpu(k,l->cores[k]->core_id,l->cores[k]->physical_core_map);
        }        
        vm->mem_init();
        vm->load_binary(l->binary_path);
        l->vm = vm;
        vm_list->push_back(vm);
    }

    auto sched_info = parser->generate_core_scheduling_order(parser->windows);
    cout << "asd" << endl;

    vector<Scheduler*> scheduler_list;
    for(auto core = 0; core<4; core++){
        Scheduler* sch = new Scheduler(&sched_info[core],core);
        scheduler_list.push_back(sch);
    }

    for(auto vm: *vm_list){
        vm->run_vm();
    }

    for(auto k = 0; k<4; k++){
        scheduler_list[k]->start();
    }
    
    for(auto sch: scheduler_list){
        sch->finish();
    }

}